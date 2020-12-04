#include "Session.hpp"

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include <cstdio>
#include <fstream>
#include <locale>
#include <string>

#include "../shared/MessageBuilder.hpp"

using namespace PDSBackup;

Session::Session(tcp::socket s) : bodyReadSoFar(0), socket(std::move(s)) {
    // inizializzo i vettori
    rawHeader.resize(Protocol::headerLength);
    bodyBuffer.resize(Protocol::bufferSize);
}

void Session::readHeader() {
    auto self(shared_from_this());

    boost::asio::async_read(
        socket,
        boost::asio::buffer(rawHeader, Protocol::headerLength),
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
    auto toRead = std::min((unsigned long long)Protocol::bufferSize, header.getBodyLenght() - bodyReadSoFar);

    std::cout << "Sto per leggere body con buffer = " << toRead << std::endl;

    socket.async_read_some(
        boost::asio::buffer(bodyBuffer, toRead),
        boost::bind(
            &Session::handleReadBody,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Session::handleReadBody(boost::system::error_code ec, std::size_t readLen) {
    std::cout << "Ho letto body con buffer = " << readLen << std::endl;

    bool isLastChunk = false;
    // controllo se ho degli errori che non sono EOF
    // oppure
    // se ho EOF ma non ho finito di leggere il body
    if ((ec && ec != boost::asio::error::eof) ||
        (ec == boost::asio::error::eof && bodyReadSoFar + readLen != header.getBodyLenght())) {
        if (ofs.is_open() && currFilePath.length() > 0) {
            ofs.close();
            boost::filesystem::remove(currFilePath);
            std::cout << "Errore ec: " << ec.message() << std::endl;
            return;
        }
    }

    // se arrivo qui vuol dire che o mi manca da leggere altro body o tutto quello
    // che ho letto e' valido (anche se eof)

    // aggiorno il contatore
    bodyReadSoFar += readLen;

    if (bodyReadSoFar == header.getBodyLenght()) isLastChunk = true;

    if (header.isFileUpload()) {
        std::cout << "handleReadBody() con file, con readLen = " << readLen << std::endl;

        std::size_t pos = body.pushWithFile(bodyBuffer, readLen);

        // faccio il controllo di validita' su pos
        if (pos != (std::size_t)-1 && pos < readLen) {
            // se ho trovato la posizione a cui comincia il file
            // allora il pushWithFile mi avra' messo il filepath
            // dentro il primo body field
            if (!ofs.is_open()) {
                currFilePath = "__ricevuti/" + body.getFields()[0];

                // creo le cartelle per ospitare il file
                boost::filesystem::path p = currFilePath;
                p.remove_filename();
                boost::filesystem::create_directories(p);

                ofs.open(currFilePath, std::ios::binary);
                // se ho dei problemi con l'apertura del file
                // FIXME da riveredere, nessuno prende questa eccezione
                if (!ofs) throw Exception::invalidFileUpload();
            };

            ofs.write(bodyBuffer.data() + pos, readLen - pos);
        }

        // se sono alla fine
        if (isLastChunk) {
            ofs.close();
            std::cout << "Ho salvato il file: " << body.getFields()[0] << std::endl;
            std::cout << "Letto tutto il messaggio!" << std::endl;

            // reset della sessione, pronti per accettare un nuovbo header
            reset();
        } else {
            // continuo a leggere
            readBody();
        };
    } else {
        body.push(bodyBuffer, readLen);

        if (isLastChunk) {
            body.parse();
            // TODO eseguire l'azione associata al messaggio

            auto fields = body.getFields();

            for (auto x : fields) std::cout << "Letto field: " << x << std::endl;

            std::cout << "Letto tutto il messaggio!" << std::endl;

            doTheStuffAndReply();

            // reset della sessione
            reset();
        } else {
            // leggo altro body
            readBody();
        }
    }
}

void Session::doRead() {
    // comincio la lettura dall'header, si occupera' lui di far partire la lettura del body
    readHeader();
}

void Session::doTheStuffAndReply() {
    // TODO da finire

    // prendo le informazioni che mi servono dal body e dall'header
    MessageBuilder msg;
    // msg.addField("Primo field");
    // msg.addField("Secondo field bellissimo!");
    // msg.setMessageCode(Protocol::MessageCode::ok);
    msg.buildWithFile("bellissimo file path.jpg", 10000);
    std::cout << "Messaggio: " + msg.buildStr() << std::endl;
}

std::string Session::printLen(std::vector<char> s, unsigned long long len) {
    return std::string(s.begin(), s.end());
}

void Session::reset(bool readNext) {
    bodyReadSoFar = 0;
    currFilePath.clear();

    if (ofs.is_open()) ofs.close();

    body.clear();
    header.clear();

    if (readNext) doRead();
}
