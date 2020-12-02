#include "Session.hpp"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include <cstdio>
#include <fstream>
#include <locale>
#include <string>

using namespace PDSBackup;

Session::Session(tcp::socket s) : bodyReadSoFar(0), socket(std::move(s)) {
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
    if (header.isFileUpload()) {
        socket.async_read_some(boost::asio::buffer(strBufBody, Protocol::bufferSize),
                               boost::bind(
                                   &Session::handleReadBodyFile,
                                   shared_from_this(),
                                   boost::asio::placeholders::error,
                                   boost::asio::placeholders::bytes_transferred));
    } else {
        boost::asio::async_read(
            socket,
            boost::asio::buffer(strBufBody, header.getBodyLenght()),
            boost::bind(
                &Session::handleReadBody,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

void Session::handleReadBody(boost::system::error_code ec, std::size_t readLen) {
    // se ho letto la quantita' giusta e non ho errore oppure sono alla fine della connessione va bene
    if (readLen == header.getBodyLenght() && (!ec || ec == boost::asio::error::eof)) {
        body.push(strBufBody, readLen);
        body.parse();
        std::vector<std::string> fields = body.getFields();
        for (auto x : fields) std::cout << "Letto field: " << x << std::endl;
        std::cout << "Letto tutto il messaggio!" << std::endl;
    }

    if (ec != boost::asio::error::eof) {
        reset();
    }
}

void Session::handleReadBodyFile(boost::system::error_code ec, std::size_t readLen) {
    bool isLastChunk = false;

    // aggiorno il contatore
    bodyReadSoFar += readLen;

    // sono all'ultimo pezzo di body se con questo chunk arrivo alla lunghezza del body
    if (bodyReadSoFar == header.getBodyLenght()) isLastChunk = true;

    // se ho un errore del tipo eof vogio controllare che sono arrivato alla fine del body
    // mi serve nel caso in cui ho una connessione aperta e ho due pacchetti uno di seguito all'altro
    // in questo modo controllo se ho chiuso la connessione e letto tutto un pacchetto
    // oppure se ho chiuso la connessine e letto parte di un pacchetto
    if (ec == boost::asio::error::eof && bodyReadSoFar != header.getBodyLenght()) {
        // chiudo eventuali file aperti
        if (ofs.is_open()) ofs.close();

        // chiudo la conessione
        // TODO si fa cosi'?
        return;
    }

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
                currFilePath = "__ricevuti/" + body.getFields()[0];

                if (!ofs.is_open()) ofs.open(currFilePath, std::ios::binary);

                // se ho dei problemi con l'apertura del file
                if (!ofs) throw Exception::invalidFileUpload();

                ofs.write(strBufBody.data() + pos, readLen - pos);
            }

            // se sono alla fine
            if (isLastChunk) {
                ofs.close();
                std::cout << "Ho salvato il file: " << body.getFields()[0] << std::endl;
                std::cout << "Letto tutto il messaggio!" << std::endl;

                // reset della sessione
                reset();
            } else {
                // continuo a leggere
                readBody();
            };

        } else {
            boost::filesystem::remove(currFilePath);
            std::cout << "Errore: " << ec.message() << std::endl;
        }

    } else {  // no ho un file upload
        // se non ho piu' dati da leggere allora devo anche chiudere il file
        if (!ec) {
            // pusho il contenuto del body e continuo a leggerlo
            body.push(strBufBody, readLen);
            readBody();

        } else if (isLastChunk || ec == boost::asio::error::eof) {
            // pusho il contenuto del body per l'ultima volta e faccio il parsing
            body.push(strBufBody, readLen);
            body.parse();

            std::vector<std::string> ciao = body.getFields();

            for (auto x : ciao) {
                std::cout << "Letto field: " << x << std::endl;
            }

            std::cout << "Letto tutto il messaggio!" << std::endl;

            // reset della sessione
            reset();
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

void Session::reset(bool readNext) {
    bodyReadSoFar = 0;
    currFilePath.clear();

    if (ofs.is_open()) ofs.close();

    body.clear();
    header.clear();

    if (readNext) doRead();
}
