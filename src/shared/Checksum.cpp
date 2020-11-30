#include "Checksum.hpp"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace PDSBackup;

std::string Checksum::md5(std::string filePath) {
    // se fa il throw di una eccezione lo gestico a livelli superiori
    std::ifstream fs(filePath, std::ios::in | std::ios::binary);

    // se sono qui il file e' aperto
    const int buffSize = 4096;
    char buf[buffSize];
    MD5_CTX ctx;
    MD5_Init(&ctx);

    do {
        fs.read(buf, buffSize);
        MD5_Update(&ctx, buf, fs.gcount());
    } while (!fs.eof());

    fs.close();

    unsigned char md[MD5_DIGEST_LENGTH];
    MD5_Final(md, &ctx);

    char hex[MD5_DIGEST_LENGTH + 1];

    std::stringstream s;

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        s << std::setfill('0') << std::setw(2) << std::hex << (md[i] & 0xff);

    return s.str();
}
