#ifndef PDS_BACKUP_SHARED_PROTOCOL
#define PDS_BACKUP_SHARED_PROTOCOL

#include <string>
#include <vector>

namespace PDSBackup {

// per poter essere vista da protocol
class Body;

class Protocol {
   public:
    // ---  Protocol constants ---
    static const std::string currentVersion;
    static const char separationChar;
    static const char messageChar;
    static const int maxBodyFields;
    static const int messageCodeLenght;
    static const int versionLenght;
    static const int bufferSize;
    static const int headerLenght;
    static const int headerBodyLenght;
    static const int sessionIdLenght;
    static const int sessionIdOffset;
    static const int bodyLenghtOffset;
    static const int messageCharOffset;
    static const int messageCodeOffset;

    enum class MessageCode {
        // client
        loginRequest = 10,
        loginCredentials = 11,
        fileProbe = 20,
        fileUpload = 21,
        fileDelete = 30,
        folderDelete = 31,

        // server
        ok = 100,
        errorGeneric = 200,
        errorLogin = 201,
        errorFailedUpload = 203,
        errorFileNotFound = 204,
        errorTransmission = 210,
        errorProtocol = 211
    };
};

}  // namespace PDSBackup
#endif