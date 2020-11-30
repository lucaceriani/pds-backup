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


using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char *argv[]) {

    //commento temporaneo
    /*

try {
    if (argc != 3) {
        std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
        return 1;
    }


    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::socket s(io_service);
    s.connect(*iterator);


    using namespace std;  // For strlen.
    // std::cout << "Enter message: ";
    // char request[max_length];
    const char *request = "0001M123--------........--------........";
    std::cout << "automessage" << std::endl;

    // std::cin.getline(request, max_length);
    size_t request_length = strlen(request);
    boost::asio::write(s, boost::asio::buffer(request, request_length));

    char reply[max_length];
    size_t reply_length = s.read_some(boost::asio::buffer(reply));
    std::cout << "Reply is: ";
    std::cout.write(reply, reply_length);
    std::cout << "\n";
} catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
}

*/

    //FileWatcher: per il momento ho impostato una cartella di prova creata dentro il progetto e commentato il codice di comunicazione con il server;

    FileWatcher fw{argv[1], std::chrono::milliseconds(5000)}; //argv[1] da modificare probabilmente con argv[3] quando si useranno anche le funzionalità di asio

    fw.start([] (std::string path_to_watch, FileStatus status) -> void {

        switch(status) {
            case FileStatus::created:
                std::cout << "File created: " << path_to_watch << '\n';

                // Codice per l' upload del file creato sul server

                //sockstream << "0100M021";
                //sockstream << "aaaabbbbccccddddeeeeffff";
                //sockstream << std::setfill('0') << std::setw(16) << path_to_watch.length() + boost::filesystem::file_size(path_to_watch);
                //sockstream << path_to_watch
                //sockstream << '\0';
                //std::ifstream fileToSend(path_to_watch, std::ios::binary)
                //sockstream << fileToSend.rdbuf();
                //sockstream.flush();

                break;
            case FileStatus::directoryCreated:
                std::cout << "Directory created: " << path_to_watch << '\n';

                // Codice per l' upload di una sottocartella della directory principale

                //sockstream << "0100M021";
                //sockstream << "aaaabbbbccccddddeeeeffff";
                //sockstream << std::setfill('0') << std::setw(16) << path_to_watch.length();
                //sockstream << path_to_watch
                //sockstream << '\0';
                //std::ifstream dirToSend(path_to_watch, std::ios::binary)
                //sockstream << dirToSend.rdbuf();
                //sockstream.flush();

                break;
            case FileStatus::modified:
                std::cout << "File modified: " << path_to_watch << '\n';

                // Codice per la modifica di un file sul server (come upload del file poi il server sovrascrive)

                //sockstream << "0100M021";
                //sockstream << "aaaabbbbccccddddeeeeffff";
                //sockstream << std::setfill('0') << std::setw(16) << path_to_watch.length() + boost::filesystem::file_size(path_to_watch);
                //sockstream << path_to_watch
                //sockstream << '\0';
                //std::ifstream fileToSend(path_to_watch, std::ios::binary)
                //sockstream << fileToSend.rdbuf();
                //sockstream.flush();

                break;
            case FileStatus::erased:
                    std::cout << "File erased: " << path_to_watch << '\n';

                // Codice per l' eliminazione di un file dal server

                //sockstream << "0100M030";
                //sockstream << "aaaabbbbccccddddeeeeffff";
                //sockstream << std::setfill('0') << std::setw(16) << path_to_watch.length() + boost::filesystem::file_size(path_to_watch);
                //sockstream << path_to_watch;

                break;
            case FileStatus::directoryErased:
                std::cout << "Directory erased: " << path_to_watch << '\n';

                // Codice per l' eliminazione di una cartella dal server

                //sockstream << "0100M031";
                //sockstream << "aaaabbbbccccddddeeeeffff";
                //sockstream << std::setfill('0') << std::setw(16) << path_to_watch.length();
                //sockstream << path_to_watch;

                break;
            default:
                std::cout << "Error! Unknown file status.\n";
        }
    });

    return 0;
}