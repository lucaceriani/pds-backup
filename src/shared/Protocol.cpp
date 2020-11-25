#include "Protocol.hpp"

#include "Body.hpp"

using namespace PDSBackup;

//  --- Definizione delle costanti ---
const std::string Protocol::currentVersion = "0001";
const char Protocol::separationChar = '\0';
const char Protocol::messageChar = 'M';
const int Protocol::maxBodyFields = 2;  // quanti campi "testuali" posso avere nel body
const int Protocol::bufferSize = 8192;

// lenghts
const int Protocol::messageCodeLenght = 3;
const int Protocol::versionLenght = 4;
const int Protocol::headerLenght = 48;
const int Protocol::headerBodyLenght = 16;
const int Protocol::sessionIdLenght = 24;

// offsets
const int Protocol::messageCharOffset = 4;
const int Protocol::messageCodeOffset = 5;
const int Protocol::sessionIdOffset = 8;
const int Protocol::bodyLenghtOffset = 32;
