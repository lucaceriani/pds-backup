//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "../shared/Exceptions.hpp"
#include "Server.hpp"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

int main(int argc, char *argv[]) {
    while (true) {
        try {
            boost::asio::io_context io_context;
            PDSBackup::Server s(io_context, 1234);
            std::cout << "Server sulla porta: 1234" << std::endl;
            io_context.run();
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << "\n";
        } catch (PDSBackup::BaseException &e) {
            std::cerr << "Errore server: " << e.what() << "\n";
        }
    }

    return 0;
}