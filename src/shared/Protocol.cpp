#include "Protocol.hpp"

using namespace PDSBackup;

// Ritorna -1 (0xFF...FF) in caso di errore altrimenti ritorna la dimensione del body
unsigned long long Protocol::parseHeader(std::size_t readLen, std::vector<char>& header, std::string& sessionId) {
    // controllo byte letti
    if (readLen != headerLenght()) return -1;

    // controllo lettera M
    if (header[4] != messageChar()) return -1;

    // controllo versione del protocollo
    for (int i = 0; i < 4; i++)
        if (header[i] != currentVersion()[i]) return -1;

    std::string bodyLenght;

    for (int i = 0; i < headerBodyLenght(); i++) {
        // controllo che tutti i caratteri successivi siano effettivamente cifre
        if (!std::isdigit(header[i + bodyLenghtOffset()])) {
            return -1;
        }
        bodyLenght.push_back(header[i + bodyLenghtOffset()]);
    }

    for (int i = 0; i < sessionIdLenght(); i++) {
        sessionId.push_back(header[i + sessionIdOffset()]);
    }

    unsigned long long retValue;

    try {
        retValue = std::stoull(bodyLenght);
    } catch (...) {
        // Se fallisco la conversione
        return -1;
    }

    return retValue;
}