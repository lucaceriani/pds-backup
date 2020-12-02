#pragma once

#include <openssl/md5.h>

#include <string>

namespace PDSBackup {
class Checksum {
   public:
    static std::string md5(std::string filePath);
};
}