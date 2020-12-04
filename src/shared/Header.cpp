#include "Header.hpp"

#include <iostream>

using namespace PDSBackup;

// Inizializzo la variabile di validita' a false
Header::Header() : valid(false) {}

bool Header::isFileUpload() {
    return messageCode == Protocol::MessageCode::fileUpload;
}

unsigned long long Header::getBodyLenght() {
    return bodyLenght;
}

std::string Header::getSessionId() {
    return sessionId;
}

bool Header::isValid() {
    return valid;
}

bool Header::parse(std::vector<char> rawHeader) {
    // imposto il valore di valid a false, in caso si dovesse chiamare piu' volte
    valid = false;

    // controllo byte letti
    if (rawHeader.size() != Protocol::headerLength) return false;

    // controllo lettera M
    if (rawHeader[Protocol::messageCharOffset] != Protocol::messageChar) return false;

    // controllo il codice del messaggio

    std::string messageCodeStr;

    for (unsigned int i = 0; i < Protocol::messageCodeLength; i++) {
        messageCodeStr.push_back(rawHeader[i + Protocol::messageCodeOffset]);
    }

    try {
        messageCode = (Protocol::MessageCode)std::stoi(messageCodeStr);
    } catch (const std::exception& e) {
        // se fallisco la conversione
        std::cerr << e.what() << std::endl;
        return false;
    }

    // controllo versione del protocollo
    for (int i = 0; i < 4; i++)
        if (rawHeader[i] != Protocol::currentVersion[i]) return false;

    std::string bodyLengthStr;

    for (unsigned int i = 0; i < Protocol::headerBodyLength; i++) {
        // controllo che tutti i caratteri successivi siano effettivamente cifre
        if (!std::isdigit(rawHeader[i + Protocol::bodyLenghtOffset])) {
            return false;
        }
        bodyLengthStr.push_back(rawHeader[i + Protocol::bodyLenghtOffset]);
    }

    for (unsigned int i = 0; i < Protocol::sessionIdLength; i++) {
        sessionId.push_back(rawHeader[i + Protocol::sessionIdOffset]);
    }

    try {
        bodyLenght = std::stoull(bodyLengthStr);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    // se sono arrivato fino qui imposto il valore di valid a true
    valid = true;
    return true;
}

void Header::clear() {
    valid = false;
    sessionId.clear();
    bodyLenght = 0;
}
