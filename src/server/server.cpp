//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <boost/asio.hpp>

#include "../shared/protocol.hpp"

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start()
    {
        do_read();
    }

private:
    tcp::socket socket_;

    enum
    {
        max_length = 1024
    };

    char data_[max_length];

    void do_read()
    {
        auto self(shared_from_this());

        // leggo l'header
        PDSBackup::Protocol p(std::move(socket_));
        if (!p.readHeader())
        {
            std::cout << "errore nella lettura dell'header" << std::endl;
        }
        else
        {
            std::cout << "header letto: " << p.stringHeader() << std::endl;
        }

        // socket_.async_read_some(boost::asio::buffer(data_, max_length), [this, self](boost::system::error_code ec, std::size_t length) {
        //     if (!ec)
        //     {

        //         // creo l'oggetto per interfacciarmi con il protocollo
        //         PDSBackup::Protocol p(soc);

        //         std::cout << "Received: " << data_ << std::endl;
        //         std::cout << "From: " << socket_.remote_endpoint().address().to_string() << std::endl;
        //         std::cout << "check >>" << p.readHeader() << std::endl;
        //     }
        // });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        // boost::asio::async_write(socket_, boost::asio::buffer(data_, length), [this, self](boost::system::error_code ec, std::size_t /*length*/) { if (!ec) { do_read(); } });
        boost::asio::async_write(socket_, boost::asio::buffer("ciao bello"), [this, self](boost::system::error_code ec, std::size_t /*length*/) { if (!ec) { do_read(); } });
    }
};

class server
{
public:
    server(boost::asio::io_context &io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) { do_accept(); }

private:
    tcp::acceptor acceptor_;

    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket))->start();
                }
                do_accept();
            });
    }
};

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        server s(io_context, std::atoi(argv[1]));
        io_context.run();
    }

    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}