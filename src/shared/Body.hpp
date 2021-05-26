#pragma once

#include <string>
#include <vector>

#include "Exceptions.hpp"
#include "Header.hpp"
#include "Protocol.hpp"

namespace PDSBackup {
class Body {
   public:
    Body();
    void setHeader(Header header);
    bool push(std::vector<char> buffer, std::size_t readLen);
    std::size_t pushWithFile(std::vector<char> buffer, std::size_t readLen);
    void parse();
    // Per ottenere i campi parsificati
    std::vector<std::string> getFields();

    void clear();

   private:
    Header header;
    std::vector<std::string> fields;
    std::vector<char> bodyBuffStorage;

};
}  // namespace PDSBackup
