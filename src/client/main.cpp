//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>


#include "FileWatcher.hpp"
#include "Client.hpp"
#include "../shared/_include.hpp"



using boost::asio::ip::tcp;

enum { max_length = 1024 };


int main(int argc, char *argv[]) {

    try {
    if (argc != 4) {
        std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
        return 1;
    }

    boost::asio::io_context io_context;
    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));


    PDSBackup::MessageBuilder mb;
    std::string message;
    std::string sessionId = "000011112222333344445555"; //fake value of sessionId


    Client c = Client(std::move(s));

    // Fase di login

    // Richiesta di login al server
    while(1){
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::loginRequest);
        mb.setSessionId(sessionId);
        message = mb.buildStr();
        boost::asio::write(s, boost::asio::buffer(message, message.length()));
        c.readHeader();
        if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok){
            std::cout << "Login authorized." << std::endl;
            c.reset();
            break;
        }
        c.manageErrors();
        c.reset();
    }

    // Invio credenziali
    while(1){
        std::string user;
        std::string pwd;
        std::cout << "Inserisci username: " << std::endl;
        std::cin >> user;
        std::cout << "Inserisci password: " << std::endl;
        std::cin >> pwd;
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::loginCredentials);
        mb.setSessionId(sessionId);
        mb.addField(user);
        mb.addField(pwd);
        message = mb.buildStr();
        boost::asio::write(s, boost::asio::buffer(message, message.length()));
        c.readHeader();
        if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok){
            std::cout << "Login ok." << std::endl;
            c.reset();
            break;
        }
        c.manageErrors();
        c.reset();
    }


    // FileWatcher

    FileWatcher fw{argv[3], std::chrono::milliseconds(5000)}; // Crea e inizializza il FileWatcher

    // Fase di probe: si verifica che i file catalogati dal FileWatcher siano già presenti sul server, altrimenti si fa l'upload

    std::vector<std::string> toBeChecked = fw.getPaths_();
    auto it = toBeChecked.begin();
    while(it != toBeChecked.end()){
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileProbe);
        mb.setSessionId(sessionId);
        mb.addField(*it);
        mb.addField(PDSBackup::Checksum::md5(*it));
        message = mb.buildStr();
        boost::asio::write(s, boost::asio::buffer(message, message.length()));
        c.readHeader();
        if(c.getMessageCode() != PDSBackup::Protocol::MessageCode::ok){
           if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::errorFileNotFound){
               std::cout << "File not found in server." << std::endl;
               // Il file non è presente sul server, lo carico
               mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileUpload);
               mb.setSessionId(sessionId);
               mb.buildWithFile(*it, boost::filesystem::file_size(*it));
               message = mb.buildStr();
               boost::asio::write(s, boost::asio::buffer(message, message.length()));
               std::ifstream file;
               file.open(*it, std::ios::binary);
               char buff[8192];
               while (file.read(buff, 8192)) {
                   boost::asio::write(s, boost::asio::buffer(buff, file.gcount()));
               }
               file.close();
               // Controllo risposta server
               c.readHeader();
               if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
                   std::cout << "File upload done." << std::endl;
               else
                   c.manageErrors();
           }else
               c.manageErrors();
        }
        c.reset();
    }

    // Avvio del FileWatcher: inizia a monitorare i cambiamenti della cartella lato client

    fw.start([&mb, &message, &sessionId, &s, &c] (std::string path_to_watch, FileStatus status) -> void {

        switch(status) {
            case FileStatus::created: {
                std::cout << "File created: " << path_to_watch << '\n';
                // Upload di un nuovo file sul server
                mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileUpload);
                mb.setSessionId(sessionId);
                mb.buildWithFile(path_to_watch, boost::filesystem::file_size(path_to_watch));
                message = mb.buildStr();
                boost::asio::write(s, boost::asio::buffer(message, message.length()));
                std::ifstream file;
                file.open(path_to_watch, std::ios::binary);
                char buff[8192];
                while (file.read(buff, 8192)) {
                    boost::asio::write(s, boost::asio::buffer(buff, file.gcount()));
                }
                file.close();
                // Controllo risposta server
                c.readHeader();
                if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
                    std::cout << "New file upload done." << std::endl;
                else
                    c.manageErrors();
                c.reset();
                break;
            }
            case FileStatus::modified:{
                std::cout << "File modified: " << path_to_watch << '\n';
                // Upload di un file modificato sul server (come upload dei nuovi file ma il server sovrascrive)
                mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileUpload);
                mb.setSessionId(sessionId);
                mb.buildWithFile(path_to_watch, boost::filesystem::file_size(path_to_watch));
                message = mb.buildStr();
                boost::asio::write(s, boost::asio::buffer(message, message.length()));
                std::ifstream file;
                file.open(path_to_watch, std::ios::binary);
                char buff[8192];
                while (file.read(buff, 8192)) {
                    boost::asio::write(s, boost::asio::buffer(buff, file.gcount()));
                }
                file.close();
                // Controllo risposta server
                c.readHeader();
                if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
                    std::cout << "Changed file upload done." << std::endl;
                else
                    c.manageErrors();
                c.reset();
                break;
            }
            case FileStatus::erased: {
                std::cout << "File erased: " << path_to_watch << '\n';
                // Eliminazione di un file dal server
                mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileDelete);
                mb.setSessionId(sessionId);
                mb.addField(path_to_watch);
                message = mb.buildStr();
                boost::asio::write(s, boost::asio::buffer(message, message.length()));
                c.readHeader();
                if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
                    std::cout << "File correctly erased from the server." << std::endl;
                else
                    c.manageErrors();
                c.reset();
                break;
            }
            case FileStatus::directoryCreated:
                std::cout << "Directory created: " << path_to_watch << '\n';
                // Nessuna comunicazione col server in questo caso
                break;
            case FileStatus::directoryErased: {
                std::cout << "Directory erased: " << path_to_watch << '\n';
                // Eliminazione di una cartella dal server
                mb.setMessageCode(PDSBackup::Protocol::MessageCode::folderDelete);
                mb.setSessionId(sessionId);
                mb.addField(path_to_watch);
                message = mb.buildStr();
                boost::asio::write(s, boost::asio::buffer(message, message.length()));
                c.readHeader();
                if(c.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
                    std::cout << "Directory correctly erased from the server." << std::endl;
                else
                    c.manageErrors();
                c.reset();
                break;
            }
            default:
                std::cout << "Error! Unknown file status.\n";
        }
    });

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}