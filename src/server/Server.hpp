#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "Session.hpp"
#include "UserCollection.hpp"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

namespace PDSBackup {

class Server {
   public:
    Server(boost::asio::io_context &io_context, short port);
    UserCollection users;

   private:
    tcp::acceptor acceptor;
    ssl::context sslCtx;

    // Accetta la connessione e la assegna ad un oggetto Session
    void doAccept();

    void loadUsers();
};
}  // namespace PDSBackup
