#ifndef PDS_BACKUP_SHARED_HEADER
#define PDS_BACKUP_SHARED_HEADER

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
    unsigned long long getBodyLenght();

   private:
    bool valid;
    std::string sessionId;
    Protocol::MessageCode messageCode;
    unsigned long long bodyLenght;
};

}  // namespace PDSBackup

#endif