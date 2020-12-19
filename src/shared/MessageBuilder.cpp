#include "MessageBuilder.hpp"

#include <iomanip>
#include <sstream>

using namespace PDSBackup;

MessageBuilder::MessageBuilder() {
    // di default il sessionId che invia indietro il server e'
    // formato da tutti trattini
    sid = std::string(Protocol::sessionIdLength, '-');
    bodyLength = 0;
}

void MessageBuilder::addField(std::string f) {
    fields.push_back(f);
    bodyLength += f.length();
}

void MessageBuilder::setMessageCode(Protocol::MessageCode m) {
    mCode = m;
}

void MessageBuilder::setSessionId(std::string sessionId) {
    sid = sessionId;
}

void MessageBuilder::buildWithFile(std::string path, std::size_t fileSize) {
    mCode = Protocol::MessageCode::fileUpload;
    bodyLength = path.length() + fileSize + 1;  // aggiungo un perche' e' il separatore
    fields.push_back(path + '\0');
};

std::string MessageBuilder::buildStr() {
    std::stringstream msg;
    unsigned long long totLen = 0;

    if (fields.size() == 0) {
        totLen = bodyLength;
    } else {
        // imposto il valore dell lunghezza del body alla sua lunghezza + i \0 che mi servono
        // quindi nel numero di field meno uno perche' l'ultimo non ce l'ha
        totLen = bodyLength + fields.size() - 1;
    }

    msg << Protocol::currentVersion;
    msg << Protocol::messageChar;
    msg << std::setfill('0') << std::setw(3) << (unsigned int)mCode;
    msg << sid;
    msg << std::setfill('0') << std::setw(Protocol::headerBodyLength) << totLen;

    // se ho dei field
    if (fields.size() > 0) {
        // mi prendo l'indice dell'ultimo elemento nei fields
        // per non appendergli infondo il \0
        std::size_t last = fields.size() - 1;

        for (std::size_t i = 0; i < fields.size(); i++) {
            msg << fields[i];
            if (i != last) msg << Protocol::separationChar;
        }
    }

    return msg.str();
}