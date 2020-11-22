
#include "Session.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <locale>
#include <string>

#include "../shared/protocol.hpp"

using namespace PDSBackup;

bool Session::readHeader() {
    auto self(shared_from_this());

    // /*
    boost::asio::async_read(
        socket, boost::asio::buffer(header, PDSB_PROT_HEADERLEN),
        [this, self](boost::system::error_code ec, std::size_t lenght) {
            if (!ec) {
                std::cout << "Letto header: " << printLen(header, lenght) << std::endl;
                unsigned long long l = checkHeader(lenght);
                if (l != -1) {
                    std::cout << "Header corretto! " << std::endl;
                    std::cout << "Leggo il body... " << std::endl;
                    readBody(l);
                } else {
                    std::cout << "Header errato!" << std::endl;
                }
            }
        });

    // */
    return true;
}

void Session::readBody(unsigned long long lenght) {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket, boost::asio::buffer(body, lenght),
        [this, self](boost::system::error_code ec, std::size_t readLen) {
            if (!ec) {
                std::cout << "Letto body: " << std::endl;
                std::cout << ">> Lunghezza: " << readLen << " B" << std::endl;

                for (unsigned long long i = 0; i < readLen; i++)
                    std::cout << HEX(body[i]);
            }
            std::cout << std::endl;
        });
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

bool Session::doRead() {
    // comincio la lettura
    return readHeader();
}

std::string Session::printLen(char* s, unsigned long long len) {
    return std::string(s).substr(0, len);
}