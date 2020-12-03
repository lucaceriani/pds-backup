#pragma once

#include <boost/asio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../shared/_include.hpp"

// per stampare i char come hex
#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << (((int)(x)) & 0xff)

using boost::asio::ip::tcp;

namespace PDSBackup {

class Session : public std::enable_shared_from_this<Session> {
   public:
    Session(tcp::socket s);

    // Metodo da chiamare per cominciare la lettura
    void doRead();

    ~Session() {
        std::cout << "Fine sessione\n" + std::string(80, '-') << std::endl;
    }

   private:
    Header header;
    Body body;

    std::vector<char> rawHeader;
    std::vector<char> bodyBuffer;
    unsigned long long bodyReadSoFar;

    tcp::socket socket;
    std::ofstream ofs;
    std::string currFilePath;

    // Legge l'header
    void readHeader();

    // Legge il body
    void readBody();

    // Si occupa di leggere il body finché non ha finito
    void handleReadBody(boost::system::error_code ec, std::size_t readLen);

    // Funzione chiamata dopo aver letto correttamente l'header
    unsigned long long checkHeader(std::size_t lenght);

    // imposta certi valori alle impostazioni iniziali
    void reset(bool readNext = true);

    // funzione che stampa un certo numero di caratteri
    std::string printLen(std::vector<char> s, unsigned long long len);
};

}  // namespace PDSBackup
