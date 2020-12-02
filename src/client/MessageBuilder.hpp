#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>

#define PROTOCOL_VERSION "0100"  // Ho considerato qui una ipotetica versione 1.0

enum class MessageType {loginRequest, loginCredentials, fileProbe, fileUpload, directoryUpload, fileDelete, directoryDelete};

class MessageBuilder {
public:
    void setVersion(std::string version);
    void setCode(MessageType messageType);
    void setSessionId(std::string sessionId);
    void setBodyLen(std::string bodyLen);
    void setPath(std::string path);
    void setChecksum(std::string checksum);
    void setUsername(std::string username);
    void setPassword(std::string password);
    std::string build();
    MessageBuilder();
private:
    std::string version;
    std::string code;
    std::string sessionId;
    std::string bodyLen;
    std::string path;
    std::string checksum;
    std::string username;
    std::string password;
    MessageType type;
};