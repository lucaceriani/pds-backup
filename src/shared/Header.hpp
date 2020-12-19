#pragma once

#include <string>
#include <vector>

#include "Protocol.hpp"

namespace PDSBackup {

class Header {
   public:
    Header();
    bool isFileUpload();
    bool parse(std::vector<char> rawHeader);
    bool isValid();
    std::string getSessionId();
    Protocol::MessageCode getMessageCode();
    unsigned long long getBodyLenght();

    void clear();

   private:
    bool valid;
    std::string sessionId;
    Protocol::MessageCode messageCode;
    unsigned long long bodyLenght;
};

}  // namespace PDSBackup
