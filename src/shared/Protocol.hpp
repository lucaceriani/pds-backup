#ifndef PDS_BACKUP_SHARED_PROTOCOL
#define PDS_BACKUP_SHARED_PROTOCOL

#include <string>
#include <vector>

namespace PDSBackup {

class Protocol {
   public:
    // ---  Protocol constants ---
    // generic
    static const std::string currentVersion;
    static const char separationChar;
    static const char messageChar;
    // lenghts
    static const int headerLenght;
    static const int headerBodyLenght;
    static const int sessionIdLenght;
    // offsets
    static const int sessionIdOffset;
    static const int bodyLenghtOffset;

    static unsigned long long parseHeader(std::size_t readLen, std::vector<char>& header, std::string& sessionId);
    static unsigned long long parseBody();
};

}  // namespace PDSBackup

#endif