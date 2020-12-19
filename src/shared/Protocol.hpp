#pragma once

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
    static const unsigned int maxBodyFields;
    static const unsigned int messageCodeLength;
    static const unsigned int versionLength;
    static const unsigned int bufferSize;
    static const unsigned int headerLength;
    static const unsigned int headerBodyLength;
    static const unsigned int sessionIdLength;
    static const unsigned int sessionIdOffset;
    static const unsigned int bodyLenghtOffset;
    static const unsigned int messageCharOffset;
    static const unsigned int messageCodeOffset;
    static const std::string sessionIdCharacters;
    static const unsigned int sessionIdValidDays;

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
