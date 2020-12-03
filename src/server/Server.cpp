
#include "Server.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "../shared/Exceptions.hpp"

using namespace PDSBackup;

Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    doAccept();
}

void Server::doAccept() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::cout << std::string(80, '-') << "\n";
            std::cout << "Nuova connessione da: " << socket.remote_endpoint().address().to_string() << std::endl;
            // try {
            std::make_shared<PDSBackup::Session>(std::move(socket))->doRead();
            // } catch (const Exception::invalidTransmission& e) {
            // socket.write_some("Trasmissione non valida");
            // } catch (...) {
            // }
        }

        // per restare in attesa di un'altra connessione
        doAccept();
    });
}