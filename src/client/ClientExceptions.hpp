#pragma once

#include <exception>
#include <stdexcept>

namespace ClientExc{

    enum class errorCode {
        // Codici identificativi per errori specifici del client
        timeoutExpiredClientEnd = 0,
        nullHeaderConnectionFailed = 1,
        expectedBodyNullBody = 2
    };

    class ClientException {
    public:
        virtual const char* what() = 0;
        virtual ClientExc::errorCode errorCode() = 0;
    };

    class timeout : public ClientException {
    public:
        const char* what();
        ClientExc::errorCode errorCode();
    };

    class connectionFailed : public ClientException {
    public:
        const char* what();
        ClientExc::errorCode errorCode();
    };

    class nullBody : public ClientException {
    public:
        const char* what();
        ClientExc::errorCode errorCode();
    };

}

