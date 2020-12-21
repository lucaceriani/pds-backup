#include "ClientUtility.hpp"

ClientUtility::ClientUtility() {}  // Costruttore

void ClientUtility::readHeader(std::vector<char> rawHeader) {
    if (header.parse(rawHeader)) {
        std::cout << "Header corretto! " << std::endl;

        if (header.getBodyLenght() != 0) {  // I messaggi dal server hanno quasi tutti body vuoto (tranne 203 e 204)
            std::cout << "Leggo il body... " << std::endl;
            body.setHeader(header);
            ClientUtility::readBody();
        }
    } else {
        std::cout << "Header errato!" << std::endl;
    }
}

void ClientUtility::readBody() {
    // FIXME
    // TODO da fixare la lettura del body

    /*
    std::vector<std::string> bodyFields = body.getFields();
     */
    // Nei messaggi del server il massimo che si può trovare in un body è il path (203 e 204)
    path = body.getFields().front();
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
}

void ClientUtility::manageErrors() {
    switch (header.getCode()) {
        case PDSBackup::Protocol::MessageCode::errorGeneric:
            std::cout << "Errore generico server." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorLogin:
            std::cout << "Errore di login." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorFailedUpload:
            std::cout << "Impossibile caricare il file." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorFileNotFound:
            std::cout << "File non presente." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorTransmission:
            std::cout << "Errore di trasmissione." << std::endl;
            break;
        case PDSBackup::Protocol::MessageCode::errorProtocol:
            std::cout << "Errore di protocollo." << std::endl;
            break;
        default:
            std::cout << "Errore sconosciuto." << std::endl;
    }
}

std::string ClientUtility::getSessionId() {
    return header.getSessionId();
}