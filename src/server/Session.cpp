
#include "Session.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdio>
#include <fstream>
#include <locale>
#include <string>

#include "../shared/Protocol.hpp"

using namespace PDSBackup;

void Session::readHeader() {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket,
        boost::asio::buffer(header, Protocol::headerLenght()),
        [this, self](boost::system::error_code ec, std::size_t readLen) {
            if (!ec) {
                std::cout << "Letto header: " << printLen(header, readLen) << std::endl;

                bodyLen = Protocol::parseHeader(readLen, header, sessionId);

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

    // TODO: cambiare il nome del file
    ofs.open("__t_received.jpg", std::ios::binary | std::ios::out);

    socket.async_read_some(boost::asio::buffer(strBufBody, 8192),
                           boost::bind(
                               &Session::handleReadBody,
                               self,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));
}

// TODO: modificare il body in modo da leggere tutti i campi in base all'header
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
        // TODO: cancellare il file
        std::cout << "Errore: " << ec.message() << std::endl;
    }
}

std::string Session::stringHeader() {
    // ritorno l'header come stringa
    return std::string(header.data());
}

void Session::doRead() {
    // comincio la lettura dall'header, si occupera' lui di far partire la lettura del body
    readHeader();
}

std::string Session::printLen(std::vector<char> s, unsigned long long len) {
    return std::string(s.data(), s.data() + len);
}
