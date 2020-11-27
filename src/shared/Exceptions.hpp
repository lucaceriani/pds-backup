#ifndef PDS_BACKUP_SHARED_EXCEPTIONS
#define PDS_BACKUP_SHARED_EXCEPTIONS

#include <exception>
#include <stdexcept>

#include "Protocol.hpp"

// tutte le eccezioni generate dal server, di fatto
// il client le gestira' in modo diverso

namespace PDSBackup {

class Exception {
    virtual const char* what();
    virtual Protocol::MessageCode messageCode();
};

class generic : public Exception {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidLogin : public Exception {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidFileUpload : public Exception {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidFile : public Exception {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidTransmission : public Exception {
    const char* what();
    Protocol::MessageCode messageCode();
};

class invalidProtocol : public Exception {
    const char* what();
    Protocol::MessageCode messageCode();
};

}  // namespace PDSBackup

#endif