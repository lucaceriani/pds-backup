#ifndef PDS_BACKUP_SHARED_EXCEPTIONS
#define PDS_BACKUP_SHARED_EXCEPTIONS

#include <exception>
#include <stdexcept>

#include "Protocol.hpp"

// tutte le eccezioni generate dal server, di fatto
// il client le gestira' in modo diverso

namespace PDSBackup {

class BaseException {
    virtual const char* what();
    virtual Protocol::MessageCode messageCode();
};

namespace Exception {

class generic : public BaseException {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidLogin : public BaseException {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidFileUpload : public BaseException {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidFile : public BaseException {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidTransmission : public BaseException {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidProtocol : public BaseException {
    const char* what();
    Protocol::MessageCode messageCode();
};
}  // namespace Exception

}  // namespace PDSBackup

#endif