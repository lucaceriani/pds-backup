
#include "Session.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdio>
#include <fstream>
#include <locale>
#include <string>

#include "../shared/protocol.hpp"

using namespace PDSBackup;

void Session::readHeader() {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket, boost::asio::buffer(header, PDSB_PROT_HEADERLEN),
        [this, self](boost::system::error_code ec, std::size_t lenght) {
            if (!ec) {
                std::cout << "Letto header: " << printLen(header, lenght) << std::endl;
                bodyLen = checkHeader(lenght);
                if (bodyLen != -1) {
                    std::cout << "Header corretto! " << std::endl;
                    std::cout << "Leggo il body... " << std::endl;
                    readBody(bodyLen);
                } else {
                    std::cout << "Header errato!" << std::endl;
                }
            }
        });
}

void Session::readBody(unsigned long long lenght) {
    auto self(shared_from_this());

    /**
     * @todo Cambiare il nome del file in quello fornito nel body
     */
    ofs.open("__t_received.jpg", std::ios::binary | std::ios::out);

    socket.async_read_some(boost::asio::buffer(strBufBody, 8192),
                           boost::bind(
                               &Session::handleReadBody,
                               self,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));
}

/**
  * @todo modificare l'handling del body
  * @body In modo da capire i vari campi a seconda dell'header
  */
void Session::handleReadBody(boost::system::error_code ec, std::size_t readLen) {
    if (!ec) {
        // scrivo tutti i dati che sono riuscto a leggere
        ofs.write(strBufBody, readLen);

        std::cout << "handleReadBody(), con readLen = " << readLen << std::endl;

        socket.async_read_some(boost::asio::buffer(strBufBody, 8192),
                               boost::bind(
                                   &Session::handleReadBody,
                                   shared_from_this(),
                                   boost::asio::placeholders::error,
                                   boost::asio::placeholders::bytes_transferred));

    } else if (ec == boost::asio::error::eof) {
        ofs.close();
        std::cout << "Letto tutto il file!" << std::endl;
    } else {
        ofs.close();
        /**
         * @todo In caso di errore cancellare il file
         */
        std::cout << "Errore: " << ec.message() << std::endl;
    }
}

std::string Session::stringHeader() {
    // ritorno l'header come stringa
    return std::string(header);
}

// Ritorna -1 (0xFF...FF) in caso di errore altrimenti ritorna la dimensione del body
unsigned long long Session::checkHeader(std::size_t lenght /* implicito il passaggio dell'header */) {
    // controllo byte letti
    if (lenght != PDSB_PROT_HEADERLEN) return -1;

    // controllo lettera M
    if (header[4] != 'M') return -1;

    // controllo versione del protocollo
    for (int i = 0; i < 4; i++)
        if (header[i] != PDSB_PROT_CURRVERSION[i]) return -1;

    // controllo che tutti i caratteri successivi siano effettivamente cifre
    std::string bodyLenght;

    for (int i = 0; i < 16; i++) {
        if (!std::isdigit(header[PDSB_PROT_BODYLEN_OFFSET]))
            return -1;

        userCode.push_back(header[i + PDSB_PROT_USERCODE_OFFSET]);
        bodyLenght.push_back(header[i + PDSB_PROT_BODYLEN_OFFSET]);
    }

    return std::stoull(bodyLenght);
}

void Session::doRead() {
    // comincio la lettura dall'header, si occupera' lui di far partire la lettura del body
    readHeader();
}

std::string Session::printLen(char* s, unsigned long long len) {
    return std::string(s).substr(0, len);
}