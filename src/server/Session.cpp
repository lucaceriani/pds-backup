#include "Session.hpp"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <cstdio>
#include <fstream>
#include <locale>
#include <string>

using namespace PDSBackup;

Session::Session(tcp::socket s) : socket(std::move(s)) {
    // inizializzo i vettori
    rawHeader.resize(Protocol::headerLenght);
    strBufBody.resize(Protocol::bufferSize);
}

void Session::readHeader() {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket,
        boost::asio::buffer(rawHeader, Protocol::headerLenght),
        [this, self](boost::system::error_code ec, std::size_t readLen) {
            if (!ec) {
                std::cout << "Letto header: " << printLen(rawHeader, readLen) << std::endl;
                std::cout << "Lunghezza: " << readLen << std::endl;

                if (header.parse(rawHeader)) {
                    std::cout << "Header corretto! " << std::endl;
                    std::cout << "Leggo il body... " << std::endl;

                    // importante
                    body.setHeader(header);

                    // parto a leggere il body
                    readBody();
                } else {
                    std::cout << "Header errato!" << std::endl;
                    // TODO inviare errore appropriato per header errato
                }
            }
        });
}

void Session::readBody() {
    // TODO: cambiare il nome del file
    // if (!ofs.is_open()) {
    //     ofs.open("__t_received.jpg", std::ios::binary | std::ios::out);
    // }

    socket.async_read_some(boost::asio::buffer(strBufBody, Protocol::bufferSize),
                           boost::bind(
                               &Session::handleReadBody,
                               shared_from_this(),
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));
}

// TODO: modificare il body in modo da leggere tutti i campi in base all'header
void Session::handleReadBody(boost::system::error_code ec, std::size_t readLen) {
    if (!ec) {
        // scrivo tutti i dati che sono riuscto a leggere
        // ofs.write(strBufBody.data(), readLen);
        std::cout << "handleReadBody(), con readLen = " << readLen << std::endl;

        // pusho il contenuto del body
        body.push(strBufBody, readLen);

        // chiamata per continuare a legger il body
        readBody();

    } else if (ec == boost::asio::error::eof) {
        // pusho il contenuto del body per l'ultima volta
        body.push(strBufBody, readLen);

        // parsifico il body
        body.parse();

        std::vector<std::string> ciao = body.getFields();

        for (auto x : ciao) {
            std::cout << "Letto field: " << x << std::endl;
        }

        std::cout << "Letto tutto il messaggio!" << std::endl;
    } else {
        // TODO: cancellare il file
        std::cout << "Errore: " << ec.message() << std::endl;
    }
}

void Session::doRead() {
    // comincio la lettura dall'header, si occupera' lui di far partire la lettura del body
    try {
        readHeader();
    } catch (...) {
        std::cout << "catch base exception" << std::endl;
        socket.write_some(boost::asio::buffer("ciao", 4));
    }
}

std::string Session::printLen(std::vector<char> s, unsigned long long len) {
    return std::string(s.begin(), s.end());
}
