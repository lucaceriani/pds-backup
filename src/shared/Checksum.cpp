#include "Checksum.hpp"

#include <openssl/evp.h>  //for all other OpenSSL function calls
#include <openssl/sha.h>  //for SHA512_DIGEST_LENGTH

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace PDSBackup;

std::string Checksum::md5(std::string filePath) {
    // se fa il throw di una eccezione lo gestico a livelli superiori
    std::ifstream fs(filePath, std::ios::in | std::ios::binary);

    if (!fs) throw std::runtime_error("Errore, impossibile aprire il file!");

    // se sono qui il file e' aperto
    const int buffSize = 65536;
    char buf[buffSize];
    MD5_CTX ctx;
    MD5_Init(&ctx);

    do {
        fs.read(buf, buffSize);
        MD5_Update(&ctx, buf, fs.gcount());
    } while (!fs.eof() || fs.fail());

    if (fs.fail()) throw std::runtime_error("Impossibile leggere il file");

    fs.close();

    unsigned char md[MD5_DIGEST_LENGTH];
    MD5_Final(md, &ctx);

    // BUG no free md5

    return toHex(md, MD5_DIGEST_LENGTH);
}

std::string Checksum::sha3(std::string input) {
    unsigned int digLen = SHA256_DIGEST_LENGTH;
    unsigned char digest[digLen];

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha3_256(), nullptr);
    EVP_DigestUpdate(ctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(ctx, digest, &digLen);
    EVP_MD_CTX_destroy(ctx);

    return toHex(digest, digLen);
}

std::string Checksum::toHex(unsigned char* bytes, std::size_t len) {
    std::stringstream s;

    for (std::size_t i = 0; i < len; i++)
        s << std::setfill('0') << std::setw(2) << std::hex << (bytes[i] & 0xff);

    return s.str();
}
