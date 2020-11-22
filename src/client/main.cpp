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
#include <fstream>
#include <iomanip>
#include <iostream>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::iostream sockstream(tcp::resolver::query{argv[1], argv[2]});

        std::ifstream ifs("./__test.jpg", std::ios::binary);

        sockstream << "0001M123";
        sockstream << "abcdefghabcdefgh";
        sockstream << std::setfill('0') << std::setw(16) << boost::filesystem::file_size("./__test.jpg");
        sockstream << ifs.rdbuf();
        sockstream.flush();

        /*
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        using namespace std;  // For strlen.
        // std::cout << "Enter message: ";
        // char request[max_length];
        const char *request = "0001M12300000000111111112222222233333333";
        std::cout << "automessage" << std::endl;

        // std::cin.getline(request, max_length);
        size_t request_length = strlen(request);
        boost::asio::write(s, boost::asio::buffer(request, request_length));

        char reply[max_length];
        size_t reply_length = s.read_some(boost::asio::buffer(reply));
        std::cout << "Reply is: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
        */

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}