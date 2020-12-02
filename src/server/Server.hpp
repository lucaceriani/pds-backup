#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "Session.hpp"

using boost::asio::ip::tcp;

namespace PDSBackup {

class Server {
   public:
    Server(boost::asio::io_context &io_context, short port);

   private:
    tcp::acceptor acceptor;

    // Accetta la connessione e la assegna ad un oggetto Session
    void doAccept();
};
}  // namespace PDSBackup
