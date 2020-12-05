#pragma once

#include <openssl/md5.h>

#include <string>

namespace PDSBackup {
class Checksum {
   public:
    static std::string md5(std::string filePath);
    static std::string sha3(std::string input);
    static std::string toHex(unsigned char* bytes, std::size_t len);
};

}