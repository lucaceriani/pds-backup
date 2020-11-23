#include "Protocol.hpp"

using namespace PDSBackup;

// Ritorna -1 (0xFF...FF) in caso di errore altrimenti ritorna la dimensione del body
unsigned long long Protocol::parseHeader(std::size_t readLen, std::vector<char>& header, std::string& sessionId) {
    // controllo byte letti
    if (readLen != Protocol::headerLenght()) return -1;

    // controllo lettera M
    if (header[4] != Protocol::messageChar()) return -1;

    // controllo versione del protocollo
    for (int i = 0; i < 4; i++)
        if (header[i] != Protocol::currentVersion()[i]) return -1;

    std::string bodyLenght;

    for (int i = 0; i < 16; i++) {
        // controllo che tutti i caratteri successivi siano effettivamente cifre
        if (!std::isdigit(header[i + Protocol::bodyLenghtOffset()]))
            return -1;

        sessionId.push_back(header[i + Protocol::sessionIdOffset()]);
        bodyLenght.push_back(header[i + Protocol::bodyLenghtOffset()]);
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