#ifndef PDS_BACKUP_SHARED_HEADER
#define PDS_BACKUP_SHARED_HEADER

#include <string>
#include <vector>

namespace PDSBackup {

class Header {
   public:
    bool parseHeader(std::vector<char> rawHeader);
    bool isValid();
    std::string getSessionId();
    int mgetMessageCode();
    unsigned long long getBodyLenght();

   private:
    bool valid;
    std::string sessionId;
    int messageCode;
    unsigned long long bodyLenght;
};

}  // namespace PDSBackup

#endif