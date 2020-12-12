#include "Session.hpp"

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include <cstdio>
#include <fstream>
#include <locale>
#include <string>

#include "../shared/Checksum.hpp"
#include "../shared/MessageBuilder.hpp"

using namespace PDSBackup;

Session::Session(tcp::socket s, UserCollection& users)
    : bodyReadSoFar(0), socket(std::move(s)), users(users) {
    // inizializzo i vettori

    rawHeader.resize(Protocol::headerLength);
    bodyBuffer.resize(Protocol::bufferSize);
}

void Session::readHeader() {
    boost::asio::async_read(
        socket,
        boost::asio::buffer(rawHeader, Protocol::headerLength),
        boost::bind(&Session::handleReadHeader,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Session::handleReadHeader(boost::system::error_code ec, std::size_t readLen) {
    // se ho errori
    // ho sempre errore se non ho letto tutto perche' faccio async_read
    if (ec) {
        std::cerr << "Errori nella lettura dell'header: " << ec.message() << std::endl;
        replyError(Protocol::MessageCode::errorTransmission);
        return;
    }

    std::cout << "Letto header: " << printLen(rawHeader, readLen) << std::endl;
    std::cout << "Lunghezza: " << readLen << std::endl;

    // provo a parsificare l'header
    if (!header.parse(rawHeader)) {
        std::cerr << "Errore nella parsificazione dell'header" << std::endl;
        if (socket.is_open()) replyError(Protocol::MessageCode::errorTransmission);
        return;
    }

    // se sono qui vuol dire che l'header e' corretto
    // ed e' stato parsificato
    std::cout << "Header corretto! " << std::endl;

    // controllo login
    std::cout << "Controllo utente" << std::endl;

    // due casi: l'utente vuole effettuare un login (se ne occupa il body)
    //   oppure  l'utente e' gia' loggato e possiede quindi un sessionId e voglio verificare che
    //           sia corretta (me ne occupo adesso)
    if (header.getMessageCode() != Protocol::MessageCode::loginCredentials) {
        // controllo la sessionid
        auto sid = users.getSessionId(header.getSessionId());
        if (!sid.has_value()) {
            std::cerr << "Errore id sesssione non valido: " << header.getSessionId() << std::endl;
            // termino la connessione
            replyError(Protocol::MessageCode::errorLogin);
            return;
        } else {
            currentUsername = sid.value().owner;
        }

        std::cout << "Id sessione ok " << std::endl;
        std::cout << "Leggo il body... " << std::endl;
        body.setHeader(header);  // importante
        readBody();
    }
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
                currFilePath = getUserPath(body.getFields()[0]);

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
            doTheStuffAndReply();
            reset();
        } else {
            // continuo a leggere
            readBody();
        };
    } else {
        body.push(bodyBuffer, readLen);

        if (isLastChunk) {
            body.parse();

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
    using MC = Protocol::MessageCode;
    MC mc = header.getMessageCode();

    if (mc == MC::loginRequest) {
        // pass
        replyOk();

    } else if (mc == MC::loginCredentials) {
        // pass

    } else if (mc == MC::fileProbe) {
        std::string sentPath = body.getFields()[0];
        std::string sentChecksum = body.getFields()[1];

        std::string myChecksum = Checksum::md5(getUserPath(sentPath));

        if (myChecksum == sentChecksum) {
            replyOk();
        } else {
            replyError(MC::errorFileNotFound, sentPath);
        }

    } else if (mc == MC::fileUpload) {
        // pass
        // se siamo qui in teoria il file e' stato caricato correttamente
        replyOk();

    } else if (mc == MC::fileDelete) {
        std::string sentPath = body.getFields()[0];

        if (boost::filesystem::remove(getUserPath(sentPath))) {
            replyOk();
        } else {
            replyError(MC::errorFileNotFound, sentPath);
        }

    } else if (mc == MC::folderDelete) {
        std::string sentPath = body.getFields()[0];

        if (boost::filesystem::remove_all(getUserPath(sentPath)) > 0) {
            replyOk();
        } else {
            replyError(MC::errorFileNotFound, sentPath);
        }
    }
}

std::string Session::printLen(std::vector<char> s, unsigned long long len) {
    return std::string(s.begin(), s.end());
}

void Session::reset(bool readNext) {
    std::cout << "Reset sessione" << std::endl;

    bodyReadSoFar = 0;
    currFilePath.clear();

    if (ofs.is_open()) ofs.close();

    body.clear();
    header.clear();

    if (readNext) doRead();
}
std::string Session::getUserPath(std::string sentPath) {
    boost::filesystem::path sp("__ricevuti/" + currentUsername + "/" + sentPath);
    return sp.string();
}

void Session::replyOk(std::string body) {
    replyError(Protocol::MessageCode::ok, body);
}

void Session::replyError(Protocol::MessageCode e, std::string body) {
    MessageBuilder msg;
    msg.setMessageCode(e);
    if (body.length() > 0) msg.addField(body);
    std::cout << "Rispondo "
              << (e == Protocol::MessageCode::ok ? "OK" : "ERROR")
              << " con body: '" << body << "'" << std::endl;
    try {
        boost::asio::write(socket, boost::asio::buffer(msg.buildStr()));
    } catch (...) {
        std::cerr << "Errore impossibile scrivere sul socket" << std::endl;
        return;
    }

    std::cout << "Risposta inviata correttamente" << std::endl;
}
