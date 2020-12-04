#include "Protocol.hpp"

#include "Body.hpp"

using namespace PDSBackup;

//  --- Definizione delle costanti ---
const std::string Protocol::currentVersion = "0001";
const char Protocol::separationChar = '\0';
const char Protocol::messageChar = 'M';
const unsigned int Protocol::maxBodyFields = 2;  // quanti campi "testuali" posso avere nel body
const unsigned int Protocol::bufferSize = 8192;

// lenghts
const unsigned int Protocol::messageCodeLength = 3;
const unsigned int Protocol::versionLength = 4;
const unsigned int Protocol::headerLength = 48;
const unsigned int Protocol::headerBodyLength = 16;
const unsigned int Protocol::sessionIdLength = 24;

// offsets
const unsigned int Protocol::messageCharOffset = 4;
const unsigned int Protocol::messageCodeOffset = 5;
const unsigned int Protocol::sessionIdOffset = 8;
const unsigned int Protocol::bodyLenghtOffset = 32;
