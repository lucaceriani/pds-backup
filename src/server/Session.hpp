#ifndef PDS_BACKUP_SERVER_SESSION
#define PDS_BACKUP_SERVER_SESSION

#include <boost/asio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "../shared/protocol.hpp"

// per stampare i char come hex
#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << (((int)(x)) & 0xff)

using boost::asio::ip::tcp;

namespace PDSBackup {

class Session : public std::enable_shared_from_this<Session> {
   public:
    Session(tcp::socket s) : socket(std::move(s)) {}

    // Metodo da hciamare per cominciare la lettura
    void doRead();

    // Ritorna l'header come stringa
    std::string stringHeader();

   private:
    char header[PDSB_PROT_HEADERLEN];
    char strBufBody[8192];
    unsigned long long bodyLen;
    std::string userCode;
    tcp::socket socket;
    std::ofstream ofs;

    // Legge l'header
    void readHeader();

    // Legge il body
    void readBody(unsigned long long lenght);

    // Si occupa di leggere il body finché non ha finito
    // @todo Controllare che la lunghezza totale del file sia corretta
    void handleReadBody(boost::system::error_code ec, std::size_t readLen);

    // Funzione chiamata dopo aver letto correttamente l'header
    unsigned long long checkHeader(std::size_t lenght);

    // funzione che stampa un certo numero di caratteri
    std::string printLen(char* s, unsigned long long len);
};

}  // namespace PDSBackup

#endif
