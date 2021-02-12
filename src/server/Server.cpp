
#include "Server.hpp"

#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace PDSBackup;

Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
      sslCtx(ssl::context::tlsv13) {
    // apro la lista degli utenti
    loadUsers();

    sslCtx.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::no_sslv3 |
        boost::asio::ssl::context::no_tlsv1_1 |
        boost::asio::ssl::context::no_tlsv1_2 |
        boost::asio::ssl::context::single_dh_use);
    sslCtx.use_certificate_chain_file("server.crt");
    sslCtx.use_private_key_file("server.pem", boost::asio::ssl::context::pem);

    doAccept();
}

void Server::doAccept() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::cout << std::string(80, '-') << "\n"
                      << "Nuova connessione da: "
                      << socket.remote_endpoint().address().to_string()
                      << std::endl;

            std::make_shared<Session>(ssl::stream<tcp::socket>(std::move(socket), sslCtx), users)
                ->start();
        }
        doAccept();
    });
}

void Server::loadUsers() {
    std::ifstream ifs;
    std::string user, salt, hash;
    ifs.open("pdsbackup.users");

    if (ifs.bad()) {
        ifs.close();
        return;
    }

    while (ifs >> user >> salt >> hash) {
        std::cout << "Caricato utente: " << user << std::endl;

        // oggetto utente implicito
        users.add({user, salt, hash}, false);
    }
}