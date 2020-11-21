#ifndef PDS_BACKUP_SERVER_SESSION
#define PDS_BACKUP_SERVER_SESSION

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "../shared/protocol.hpp"

using boost::asio::ip::tcp;

namespace PDSBackup {

class Session : public std::enable_shared_from_this<Session> {
   public:
    Session(tcp::socket s) : socket(std::move(s)) {}

    bool doRead();
    std::string stringHeader();

   private:
    char header[PDSB_PROT_HEADERLEN];
    tcp::socket socket;

    bool readHeader();

    // Funzione chiamata dopo aver letto correttamente l'header
    bool checkHeader(std::size_t lenght);
};

}  // namespace PDSBackup

#endif
