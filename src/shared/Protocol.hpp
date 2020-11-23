#ifndef PDS_BACKUP_SHARED_PROTOCOL
#define PDS_BACKUP_SHARED_PROTOCOL

#include <string>
#include <vector>

namespace PDSBackup {

class Protocol {
   public:
    // ---  Protocol constants ---
    // generic
    static std::string currentVersion() { return "0001"; }
    static char separationChar() { return '\0'; }
    static char messageChar() { return 'M'; }
    // lenghts
    static int headerLenght() { return 48; }
    static int sessionIdLenght() { return 24; }
    // offsets
    static int sessionIdOffset() { return 8; }
    static int bodyLenghtOffset() { return 32; }

    static unsigned long long parseHeader(std::size_t readLen, std::vector<char>& header, std::string& sessionId);
    static unsigned long long parseBody();
};

}  // namespace PDSBackup

#endif