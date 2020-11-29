#pragma once

#include <string>
#include <vector>

#include "Exceptions.hpp"
#include "Header.hpp"
#include "Protocol.hpp"

namespace PDSBackup {
class Body {
   public:
    bool needsToSaveFile();
    void setHeader(Header header);
    bool push(std::vector<char> buffer, std::size_t readLen);
    bool parse();
    // Per ottenere i campi parsificati
    std::vector<std::string> getFields();

    Body() {
        // inizializzazione buffer (ottimizzazione)
        bodyBuffStorage.reserve(Protocol::bufferSize);
    }

   private:
    Header header;
    std::vector<std::string> fields;
    std::vector<char> bodyBuffStorage;

    bool parseWithBody(std::vector<char> buffer);
};
}  // namespace PDSBackup
