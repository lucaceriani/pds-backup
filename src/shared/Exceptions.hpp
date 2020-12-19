#pragma once

#include <exception>
#include <stdexcept>

#include "Protocol.hpp"

// tutte le eccezioni generate dal server, di fatto
// il client le gestira' in modo diverso

namespace PDSBackup {

class BaseException {
   public:
    virtual const char* what() = 0;
    virtual Protocol::MessageCode messageCode() = 0;
};

namespace Exception {

class generic : public BaseException {
   public:
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidLogin : public BaseException {
   public:
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidFileUpload : public BaseException {
   public:
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidFile : public BaseException {
   public:
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidTransmission : public BaseException {
   public:
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidProtocol : public BaseException {
   public:
    const char* what();
    Protocol::MessageCode messageCode();
};
}  // namespace Exception

}  // namespace PDSBackup
