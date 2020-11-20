#include "Session.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "../shared/protocol.hpp"

using namespace PDSBackup;

bool Session::readHeader() {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket, boost::asio::buffer(header, PDSB_PROT_HEADERLEN),
        [this, self](boost::system::error_code ec, std::size_t lenght) {
            if (!ec) {
                std::cout << "Letto header: " << header << std::endl;
                if (checkHeader(lenght)) {
                    std::cout << "Header corretto! " << std::endl;
                } else {
                    std::cout << "Header errato!" << std::endl;
                }
            }
        });

    return true;
}

std::string Session::stringHeader() {
    // ritorno l'header come stringa
    return std::string(header);
}

bool Session::checkHeader(std::size_t lenght /* implicito il passaggio dell'header */) {
    // controllo byte letti
    if (lenght != PDSB_PROT_HEADERLEN) return false;

    // controllo lettera M
    if (header[4] != 'M') return false;

    // controllo versione del protocollo
    for (int i = 0; i < 4; i++)
        if (header[i] != PDSB_PROT_CURRVERSION[i]) return false;

    

    return true;
}

bool Session::doRead() {
    // comincio la lettura
    return readHeader();
}