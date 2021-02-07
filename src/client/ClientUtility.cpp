#include "ClientUtility.hpp"

ClientUtility::ClientUtility() {}

unsigned long long ClientUtility::readHeader(std::vector<char> rawHeader) {
    if (header.parse(rawHeader)){
        std::cout << "Header corretto! " << std::endl;
        if (header.getBodyLenght() != 0) {  // I messaggi dal server hanno quasi tutti body vuoto (tranne 203 e 204)
            std::cout << "Leggo il body... " << std::endl;
            body.setHeader(header);
        }
    }else{
        std::cout << "Header errato!" << std::endl;
    }
    return header.getBodyLenght();
}

void ClientUtility::readBody(std::vector<char> rawBody) {
    // Nei messaggi del server il massimo che si può trovare in un body è il path (203 e 204)
    body.push(rawBody, rawBody.size());
    body.parse();
    path = body.getFields().front();
    std::cout << path << std::endl;
}

PDSBackup::Protocol::MessageCode ClientUtility::getMessageCode() {
    return header.getCode();
}

std::string ClientUtility::printLen(std::vector<char> s) {
    return std::string(s.begin(), s.end());
}

void ClientUtility::reset() {
    body.clear();
    header.clear();
    path = "";
}

void ClientUtility::manageErrors() {
    switch (header.getCode()) {
        case PDSBackup::Protocol::MessageCode::errorGeneric:
            throw PDSBackup::Exception::generic();
        case PDSBackup::Protocol::MessageCode::errorLogin:
            std::cout << "Errore di login, riprova." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorFailedUpload:
            throw PDSBackup::Exception::invalidFileUpload();
        case PDSBackup::Protocol::MessageCode::errorFileNotFound:
            std::cout << "File non presente / file di dimensione 0b eliminato correttamente." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorTransmission:
            throw PDSBackup::Exception::invalidTransmission();
        case PDSBackup::Protocol::MessageCode::errorProtocol:
            throw PDSBackup::Exception::invalidProtocol();
        default:
            throw std::string("Errore sconosciuto");
    }
}

std::string ClientUtility::getSessionId() {
    return header.getSessionId();
}