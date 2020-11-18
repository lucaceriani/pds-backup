#include <string>

#include "protocol.hpp"

using namespace PDSBackup;
bool Protocol::readHeader() {

    // ;ettura asincrona
    header = std::string("ok");

    return true;
}

std::string Protocol::stringHeader() {
    return header;
}