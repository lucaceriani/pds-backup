
#include "Server.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace PDSBackup;

Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    // apro la lista degli utenti

    users.add({"gbkmZSAiIWSKnYDuuTcqcIqQ",
               "20211205T145130",
               "pippo",
               // pass=luca
               "sale",
               "92523ace95131bacb5d7666914596be087007f25373500860629db99839d64ba"});

    doAccept();
}

void Server::doAccept() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::cout << std::string(80, '-') << "\n";
            std::cout << "Nuova connessione da: " << socket.remote_endpoint().address().to_string() << std::endl;
            std::make_shared<Session>(std::move(socket))->doRead();
        }
        doAccept();
    });
}