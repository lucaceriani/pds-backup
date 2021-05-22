#include "ClientUtility.hpp"

// Costruttore dell'oggetto ClientUtility.

ClientUtility::ClientUtility() {}

// Funzione che parsifica l'header e se presente, legge il body. In caso di problemi restituisce un messaggio di errore.

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

// Funzione che si occupa di leggere il body, parsificarlo e salvarne i campi utili.

void ClientUtility::readBody(std::vector<char> rawBody) {
    // Nei messaggi del server il massimo che si può trovare in un body è il path (203 e 204)
    body.push(rawBody, rawBody.size());
    body.parse();
    path = body.getFields().front();
    std::cout << path << std::endl;
}

// Getter per il MessageCode contenuto nell'header.

PDSBackup::Protocol::MessageCode ClientUtility::getMessageCode() {
    return header.getCode();
}

// Stampa su stringa.

std::string ClientUtility::printLen(std::vector<char> s) {
    return std::string(s.begin(), s.end());
}

// Effettua un reset per gli oggetti body ed header in modo da poterli riutilizzare.

void ClientUtility::reset() {
    body.clear();
    header.clear();
    path = "";
}

// Funzione che si occupa di gestire i diversi messaggi di errore che potrebbero arrivare dal server.
// In caso di errore lancia l'eccezione più opportuna o in alcuni casi stampa semplicemente un messaggio di errore.

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

// Getter per il sessionId.

std::string ClientUtility::getSessionId() {
    return header.getSessionId();
}