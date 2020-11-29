#include "Body.hpp"

#include <stdexcept>

#include "Header.hpp"

using namespace PDSBackup;

void Body::setHeader(Header h) {
    header = h;
}

bool Body::push(std::vector<char> buffer, std::size_t readLen) {
    if (header.isFileUpload()) {
        // TODO push if header is fileupload
    } else {
        for (int i = 0; i < readLen; i++) {
            bodyBuffStorage.push_back(buffer[i]);
        }
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

    for (int i = 0; i < bodyBuffStorage.size(); i++) {
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