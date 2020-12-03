#pragma once

#include <string>
#include <vector>

#include "Protocol.hpp"

namespace PDSBackup {

class MessageBuilder {
   public:
    MessageBuilder();
    void addField(std::string f);
    void setMessageCode(Protocol::MessageCode m);
    void setSessionId(std::string sessionId);

    // crea la stringa da inviare
    std::vector<char> build();
    std::string buildStr();

   private:
    std::vector<std::string> fields;
    std::string sid;
    Protocol::MessageCode mCode;
    unsigned long long bodyLength;
};

}  // namespace PDSBackup
