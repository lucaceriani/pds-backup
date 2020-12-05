#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "Session.hpp"
#include "UserCollection.hpp"

using boost::asio::ip::tcp;

namespace PDSBackup {

class Server {
   public:
    Server(boost::asio::io_context &io_context, short port);
    UserCollection users;

   private:
    tcp::acceptor acceptor;

    // Accetta la connessione e la assegna ad un oggetto Session
    void doAccept();
};
}  // namespace PDSBackup
