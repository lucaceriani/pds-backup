
#include "Server.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <string>

using namespace PDSBackup;

Server::Server(boost::asio::io_context &io_context, short port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    doAccept();
}

void Server::doAccept() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::cout << "Nuova connessione da: " << socket.remote_endpoint().address().to_string() << std::endl;
            std::make_shared<PDSBackup::Session>(std::move(socket))->doRead();
        }

        // per restare in attesa di un'altra connessione
        doAccept();
    });
}