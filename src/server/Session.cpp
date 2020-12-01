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
    socket.async_read_some(boost::asio::buffer(strBufBody, Protocol::bufferSize),
                           boost::bind(
                               &Session::handleReadBody,
                               shared_from_this(),
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));
}

void Session::handleReadBody(boost::system::error_code ec, std::size_t readLen) {
    if (header.isFileUpload()) {
        // se non ho errori o se non ho piu' dati da leggere
        if (!ec || ec == boost::asio::error::eof) {
            std::cout << "handleReadBody() con file, con readLen = " << readLen << std::endl;

            std::size_t pos = body.pushWithFile(strBufBody, readLen);

            // faccio il controllo di validita' su pos
            if (pos != (std::size_t)-1 && pos < readLen) {
                // se ho trovato la posizione a cui comincia il file
                // allora il pushWithFile mi avra' messo il filepath
                // dentro il primo body field
                if (!ofs.is_open()) ofs.open(body.getFields()[0], std::ios::out | std::ios::binary);

                // se ho dei problemi con l'apertura del file
                if (!ofs) throw Exception::invalidFileUpload();

                ofs.write(strBufBody.data() + pos, readLen - pos);
            }

            // se non ho finito di geggere leggo altrimenti no
            if (ec == boost::asio::error::eof) {
                ofs.close();
                std::cout << "Ho salvato il file: " << body.getFields()[0] << std::endl;
                std::cout << "Letto tutto il messaggio!" << std::endl;
            } else {
                readBody();
            };

        } else {
            // TODO: cancellare il file
            std::cout << "Errore: " << ec.message() << std::endl;
        }

    } else {  // no ho un file upload
        // se non ho piu' dati da leggere allora devo anche chiudere il file
        if (!ec) {
            // pusho il contenuto del body e continuo a leggerlo
            body.push(strBufBody, readLen);
            readBody();

        } else if (ec == boost::asio::error::eof) {
            // pusho il contenuto del body per l'ultima volta e faccio il parsing
            body.push(strBufBody, readLen);
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
