#include "Client.hpp"

Client::Client(tcp::socket s) : socket(std::move(s)){    // Costruttore
    bodyReadSoFar = 0;
    rawHeader.resize(PDSBackup::Protocol::headerLength);
}

void Client::readHeader() {
            boost::asio::async_read(socket, boost::asio::buffer(rawHeader, PDSBackup::Protocol::headerLength),

            [this](boost::system::error_code ec, std::size_t readLen) {
                if (!ec) {
                    std::cout << "Letto header: " << printLen(rawHeader) << std::endl;
                    std::cout << "Lunghezza: " << readLen << std::endl;

                    if (header.parse(rawHeader)) {
                        std::cout << "Header corretto! " << std::endl;
                        std::cout << "Leggo il body... " << std::endl;

                        if(header.getBodyLenght() != 0) {  // I messaggi dal server hanno quasi tutti body vuoto (tranne 203 e 204)
                            body.setHeader(header);
                            Client::readBody();
                        }

                    } else {
                        std::cout << "Header errato!" << std::endl;
                    }
                }
            });
}

void Client::readBody() {
    auto toRead = std::min((unsigned long long) PDSBackup::Protocol::bufferSize, header.getBodyLenght() - bodyReadSoFar);

    std::cout << "Sto per leggere body con buffer = " << toRead << std::endl;

    // Nei messaggi del server il massimo che si può trovare in un body è il path (203 e 204)
    Client::path = Client::body.getFields().front();
}

PDSBackup::Protocol::MessageCode Client::getMessageCode(){
    return header.getCode();
}

std::string Client::printLen(std::vector<char> s) {
    return std::string(s.begin(), s.end());
}

void Client::reset(){
    bodyReadSoFar = 0;

    body.clear();
    header.clear();
}

void Client::manageErrors(){
    switch(header.getCode()){
        case PDSBackup::Protocol::MessageCode::errorGeneric:
            std::cout << "Generic error." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorLogin:
            std::cout << "Login error." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorFailedUpload:
            std::cout << "Can not upload file." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorFileNotFound:
            std::cout << "File not found." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorTransmission:
            std::cout << "Transmission error." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorProtocol:
            std::cout << "Protocol error." << std::endl;
            break;
        default:
            std::cout << "Unknown error." << std::endl;
    }
}

std::string Client::getSessionId(){
    return header.getSessionId();
}