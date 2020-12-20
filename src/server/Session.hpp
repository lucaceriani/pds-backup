#pragma once

#include <boost/asio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../shared/_include.hpp"
#include "UserCollection.hpp"

using boost::asio::ip::tcp;

namespace PDSBackup {

class Session : public std::enable_shared_from_this<Session> {
   public:
    Session(tcp::socket s, UserCollection& users);

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

    UserCollection& users;
    std::string currentUsername;

    // Legge l'header
    void readHeader();
    void handleReadHeader(boost::system::error_code ec, std::size_t readLen);

    // Legge il body
    void readBody();

    // Si occupa di leggere il body finché non ha finito
    void handleReadBody(boost::system::error_code ec, std::size_t readLen);

    // Fa l'azione richiesta dal server (tranne salvare il file) e risponde
    void doTheStuffAndReply();

    std::string getUserPath(std::string relPath);

    // risponde OK, nel caso in cui ho fatto il login ho un sessionId
    void replyOk(std::string sessionId = "");

    // risponde con un errore
    void replyError(Protocol::MessageCode e, std::string body = "");

    // Imposta certi valori alle impostazioni iniziali
    void reset(bool readNext = true);

    // funzione che stampa un certo numero di caratteri
    std::string printLen(std::vector<char> s, unsigned long long len);
};

}  // namespace PDSBackup
