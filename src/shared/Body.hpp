#ifndef PDS_BACKUP_SHARED_BODY
#define PDS_BACKUP_SHARED_BODY

#include "Protocol.hpp"

namespace PDSBackup {
class Body {
   public:
    bool needsToSaveFile();

    Body() {
        // inizializzazione buffer
        bodyBuf.resize(Protocol::bufferSize);
    }

   private:
    std::vector<char> bodyBuf;

    // Per ottenere i campi parsificati
    std::vector<std::string> getFields();
};
}  // namespace PDSBackup

#endif