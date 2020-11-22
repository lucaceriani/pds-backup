#ifndef PDS_BACKUP_SERVER_SESSION
#define PDS_BACKUP_SERVER_SESSION

#include <boost/asio.hpp>
#include <iomanip>
#include <memory>
#include <string>

#include "../shared/protocol.hpp"

#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << (((int)(x)) & 0xff)

using boost::asio::ip::tcp;

namespace PDSBackup {

class Session : public std::enable_shared_from_this<Session> {
   public:
    Session(tcp::socket s) : socket(std::move(s)) {}

    bool doRead();
    std::string stringHeader();

   private:
    char header[PDSB_PROT_HEADERLEN];
    char body[1024000];
    std::string userCode;
    tcp::socket socket;

    bool readHeader();
    void readBody(unsigned long long lenght);

    // Funzione chiamata dopo aver letto correttamente l'header
    unsigned long long checkHeader(std::size_t lenght);

    std::string printLen(char* s, unsigned long long len);
};

}  // namespace PDSBackup

#endif
