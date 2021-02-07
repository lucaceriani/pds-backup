#include "Exceptions.hpp"

using namespace PDSBackup;
using namespace PDSBackup::Exception;

const char* generic::what() { return "Errore generico server"; }
Protocol::MessageCode generic::messageCode() { return Protocol::MessageCode::errorGeneric; }

const char* invalidLogin::what() { return "Errore di autenticazione"; }
Protocol::MessageCode invalidLogin::messageCode() { return Protocol::MessageCode::errorLogin; }

const char* invalidFileUpload::what() { return "Errore nel salvare il file sul server"; }
Protocol::MessageCode invalidFileUpload::messageCode() { return Protocol::MessageCode::errorFailedUpload; }

const char* invalidFile::what() { return "Errore file non presente / checksum errato"; }
Protocol::MessageCode invalidFile::messageCode() { return Protocol::MessageCode::errorFileNotFound; }

const char* invalidTransmission::what() { return "Errore nella trasmissione del client"; }
Protocol::MessageCode invalidTransmission::messageCode() { return Protocol::MessageCode::errorTransmission; }

const char* invalidProtocol::what() { return "Errore versione del protocollo client differente"; }
Protocol::MessageCode invalidProtocol::messageCode() { return Protocol::MessageCode::errorProtocol; }
