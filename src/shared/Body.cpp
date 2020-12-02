#include "Body.hpp"

#include <stdexcept>

#include "Header.hpp"

using namespace PDSBackup;

Body::Body() {
    // inizializzazione buffer (ottimizzazione)
    bodyBuffStorage.reserve(Protocol::bufferSize);
    fields.reserve(Protocol::maxBodyFields);
}

void Body::setHeader(Header h) {
    header = h;
}

// ritorno -1 se nel buffer non c'e' il percorso del file e me ne serve un altro
// ritorno la poszione da cui partire a prender il file, generalemente diversa da 0 per la prima
// e poi sempre 0 chiaramente.
std::size_t Body::pushWithFile(std::vector<char> buffer, std::size_t readLen) {
    // se ho gia' settato il primo (e unico in questo caso) field,
    // allora posso ritornare zero tranquillamente
    if (fields.size() == 1) return 0;

    //altrimenti
    push(buffer, readLen);

    // creo una stringa a partire dal buffer
    std::string buffStr(bodyBuffStorage.begin(), bodyBuffStorage.end());

    // cerco la posizione del carattere nullo
    std::size_t pos = buffStr.find('\0');

    if (pos != std::string::npos) {
        // se lo trovo
        // pos sarebbe la posizione, ma substr vuole il numero di caratteri
        // in questo caso mi va bene perche' se vado avanti da 0 a pos caratteri
        // arrivo al carattere prima di \0
        fields.push_back(buffStr.substr(0, pos));

        // nel caso estremo in cui il buffer finisce esattamente con il carattere \0
        // ritorno comunque pos+1 in modo da segnalare al chiamante il fatto che
        // non c'e' nulla da scrivere
        return pos + 1;

    } else {
        // se sono qui e' perche' non ho trovato il filepath nel buffer e quindi
        // ne devo leggere un altro. Essendo il tipo di ritorno size_t il -1
        // sara' convertito a MAX_SIZE, che va bene per i nostri scopi.
        return -1;
    }
}

bool Body::push(std::vector<char> buffer, std::size_t readLen) {
    for (size_t i = 0; i < readLen; i++) {
        bodyBuffStorage.push_back(buffer[i]);
    }

    // controllo che il push non superi il valore di bodylenght dell'header
    if (bodyBuffStorage.size() > header.getBodyLenght()) {
        throw Exception::invalidTransmission();
        return false;
    }

    return true;
}

bool Body::parse() {
    //controlli di base
    if (!header.isValid()) throw;
    // if (header.isFileUpload()) parseWithBody(bodyBuffStorage);

    int lastPos = 0;

    for (size_t i = 0; i < bodyBuffStorage.size(); i++) {
        if (bodyBuffStorage[i] == '\0') {
            fields.push_back(std::string(bodyBuffStorage.data() + lastPos, bodyBuffStorage.data() + i));
            lastPos = i + 1;
        }
    }

    // inserico l'ultimo campo (o anche l'unico) che non ha '\0' in fondo
    fields.push_back(std::string(bodyBuffStorage.data() + lastPos,
                                 bodyBuffStorage.data() + bodyBuffStorage.size()));

    // TODO da modificare il return di Body::parse()
    return true;
}

bool Body::parseWithBody(std::vector<char> buffer) {
    // TODO parseWithBody
    return true;
}

std::vector<std::string> Body::getFields() {
    return fields;
}