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
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/filesystem.hpp>

#include "FileWatcher.hpp"
#include "MessageBuilder.hpp"
//#include "../shared/Checksum.hpp"


using boost::asio::ip::tcp;

enum { max_length = 1024 };


int main(int argc, char *argv[]) {

    try {
    if (argc != 4) {
        std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
        return 1;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::socket s(io_service);
    s.connect(*iterator);
    tcp::iostream sockstream(tcp::resolver::query{argv[1], argv[2]});


    // Sezione per il login



    std::string notAuthenticatedId = "000000000000000000000000";
    std::string sessionId = "111122223333444455556666"; //fake session Id
    MessageBuilder mb;
    std::string message;

    /*
    // Fase di login (aggiungere le risposte del server e i controlli)

    do{
    // Richiesta di login al server
    mb.setCode(MessageType::loginRequest);
    mb.setSessionId(notAuthenticatedId);  // SessionId provvisorio
    mb.setBodyLen("0");
    message = mb.build();
    sockstream << message;
    sockstream.flush();

    // Server invia risposta
    // Controlla header risposta
    // Se risposta "ok/procedi" -> procedi all' inserimento delle credenziali
    // Se risposta negativa riformula la richiesta (ciclo do-while)
    }while(1);

    do{
    // Invio credenziali
    std::string user;
    std::string pwd;
    std::cout << "Inserisci username: " << std::endl;
    std::cin >> user;
    std::cout << "Inserisci password: " << std::endl;
    std::cin >> pwd;
    mb.setCode(MessageType::loginCredentials);
    mb.setSessionId(sessionId);
    mb.setUsername(user);
    mb.setPassword(pwd);
    mb.setBodyLen(std::to_string(user.length() + pwd.length() + 1));
    message = mb.build();
    sockstream << message;
    sockstream.flush();

    // Server invia risposta
    // Controlla header risposta
    // Se risposta "ok/procedi" passa alla fase di probe
    // Se rispost negativa reinserisci le credenziali corrette (do-while)
    }while(1);
    */



    /*
    // Probe del file (usato all' avvio del client dopo l' autenticazione, non ho ancora deciso dove metterlo)

    mb.setCode(MessageType::fileProbe);
    mb.setSessionId(sessionId);
    mb.setBodyLen(std::to_string(path_to_watch.length() + PDSBackup::Checksum::md5().length()));
    mb.setPath(path_to_watch);
    mb.setChecksum(PDSBackup::Checksum::md5("")); //da inserire il percorso del file da verificare
    message = mb.build();
    sockstream << message;
    sockstream.flush();

    */

    //FileWatcher

    FileWatcher fw{argv[3], std::chrono::milliseconds(5000)};

    fw.start([&mb, &message, &sessionId, &sockstream] (std::string path_to_watch, FileStatus status) -> void {

        switch(status) {
            case FileStatus::created: {
                std::cout << "File created: " << path_to_watch << '\n';
                // Codice per l' upload del file creato sul server


                mb.setCode(MessageType::fileUpload);
                mb.setSessionId(sessionId);
                mb.setBodyLen(std::to_string(path_to_watch.length() + boost::filesystem::file_size(path_to_watch) + 1));
                mb.setPath(path_to_watch);
                message = mb.build();
                sockstream << message;
                char *buff;
                std::ifstream fileToSend(path_to_watch, std::ios::binary);
                while (fileToSend.read(buff, 8000)) {
                    sockstream << buff;
                }
                sockstream.flush();

                break;
            }
            case FileStatus::directoryCreated: {
                std::cout << "Directory created: " << path_to_watch << '\n';
                // Codice per l' upload di una sottocartella della directory principale

                mb.setCode(MessageType::directoryUpload);  // In realtà non viene fatto un upload effettivo, si crea solo la cartella sul server al path corrispondente
                mb.setSessionId(sessionId);
                mb.setBodyLen(std::to_string(path_to_watch.length()));
                mb.setPath(path_to_watch);
                message = mb.build();
                sockstream << message;
                sockstream.flush();

                break;
            }
            case FileStatus::modified:{
                std::cout << "File modified: " << path_to_watch << '\n';
                // Codice per la modifica di un file sul server (come upload del file poi il server sovrascrive)

                mb.setCode(MessageType::fileUpload);
                mb.setSessionId(sessionId);
                mb.setBodyLen(std::to_string(path_to_watch.length() + boost::filesystem::file_size(path_to_watch) + 1));
                mb.setPath(path_to_watch);
                message = mb.build();
                sockstream << message;
                char* buff;
                std::ifstream fileToSend(path_to_watch, std::ios::binary);
                while(fileToSend.read(buff,8000)){
                    sockstream << buff;
                }
                sockstream.flush();

                break;
            }
            case FileStatus::erased: {
                std::cout << "File erased: " << path_to_watch << '\n';
                // Codice per l' eliminazione di un file dal server

                mb.setCode(MessageType::fileDelete);
                mb.setSessionId(sessionId);
                mb.setBodyLen(std::to_string(path_to_watch.length()));
                mb.setPath(path_to_watch);
                message = mb.build();
                sockstream << message;
                sockstream.flush();

                break;
            }
            case FileStatus::directoryErased: {
                std::cout << "Directory erased: " << path_to_watch << '\n';
                // Codice per l' eliminazione di una cartella dal server

                mb.setCode(MessageType::directoryDelete);
                mb.setSessionId(sessionId);
                mb.setBodyLen(std::to_string(path_to_watch.length()));
                mb.setPath(path_to_watch);
                message = mb.build();
                sockstream << message;
                sockstream.flush();

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