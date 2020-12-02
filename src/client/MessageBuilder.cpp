#include "MessageBuilder.hpp"

MessageBuilder::MessageBuilder() {}

void MessageBuilder::setVersion(std::string version){
    this->version = version;
}

void MessageBuilder::setCode(MessageType messageType){
    this->type = messageType;
    switch(messageType){
        case MessageType::loginRequest:
            this->code = "M010";
            break;
        case MessageType::loginCredentials:
            this->code = "M011";
            break;
        case MessageType::fileProbe:
            this->code = "M020";
            break;
        case MessageType::fileUpload:
            this->code = "M021";
            break;
        case MessageType::directoryUpload:
            this->code = "M022";
            break;
        case MessageType::fileDelete:
            this->code = "M030";
            break;
        case MessageType::directoryDelete:
            this->code = "M031";
            break;
        default:
            std::cout << "Error! Unknown message Type.\n";
    }
}

void MessageBuilder::setSessionId(std::string sessionId){
    this->sessionId = sessionId;
}

void MessageBuilder::setBodyLen(std::string bodyLen){
    std::string tmp = bodyLen;
    tmp.insert(tmp.begin(), 16 - tmp.size(), '0');
    this->bodyLen = tmp;
}

void MessageBuilder::setPath(std::string path){
    this->path = path;
}

void MessageBuilder::setChecksum(std::string checksum) {
    this->checksum = checksum;
}

void MessageBuilder::setUsername(std::string username) {
    this->username = username;
}

void MessageBuilder::setPassword(std::string password) {
    this->password = password;
}

std::string MessageBuilder::build() {
    std::string header;
    std::string body;
    std::string message;
    setVersion(PROTOCOL_VERSION);
    header = this->version + this->code + this->sessionId + this->bodyLen;
    switch (this->type) {
        case MessageType::loginRequest:
            body = "";
            break;
        case MessageType::loginCredentials:
            body = this->username + '\0' + this->password;
            break;
        case MessageType::fileProbe:
            body = this->path + '\0' + this->checksum;
            break;
        case MessageType::fileUpload:
            body = this->path + '\0';
            break;
        case MessageType::directoryUpload:
            body = this->path + '\0';
            break;
        case MessageType::fileDelete:
            body = this->path;
            break;
        case MessageType::directoryDelete:
            body = this->path;
            break;
        default:
            std::cout << "Error! Unknown header code.\n";
    }
    message = header + body;
    return message;
}