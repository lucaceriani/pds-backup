#include "Client.hpp"

Client::Client(tcp::socket socket) : socket(std::move(socket)){} // Costruttore

void Client::readHeader() {
            boost::asio::read(stream, boost::asio::buffer(rawHeader, PDSBackup::Protocol::headerLength),

            [this](boost::system::error_code ec, std::size_t readLen) {
                if (!ec) {
                    std::cout << "Letto header: " << printLen(rawHeader) << std::endl;
                    std::cout << "Lunghezza: " << readLen << std::endl;

                    if (header.parse(rawHeader)) {
                        std::cout << "Header corretto! " << std::endl;
                        std::cout << "Leggo il body... " << std::endl;

                        body.setHeader(header);

                        if(header.getBodyLenght() != 0)  // I messaggi dal server hanno quasi tutti body vuoto (tranne 203 e 204)
                            readBody();

                    } else {
                        std::cout << "Header errato!" << std::endl;
                    }
                }
            });
}

void Client::readBody() {
    auto toRead = std::min((unsigned long long) PDSBackup::Protocol::bufferSize, header.getBodyLenght() - bodyReadSoFar);

    std::cout << "Sto per leggere body con buffer = " << toRead << std::endl;

    //aggiungere lettura del body
}


std::string Client::printLen(std::vector<char> s) {
    return std::string(s.begin(), s.end());
}