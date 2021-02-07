#include "ClientExceptions.hpp"

using namespace ClientExc;

const char* timeout::what() { return "Tentativo di riconnessione fallito per 10 minuti, il client termina."; }
errorCode timeout::errorCode() { return errorCode::timeoutExpiredClientEnd; }

const char* connectionFailed::what() { return "La connessione con il server è caduta."; }
errorCode connectionFailed::errorCode() { return errorCode::nullHeaderConnectionFailed; }

const char* nullBody::what() { return "Atteso messaggio con body non nullo, ricevuto messaggio con body nullo."; }
errorCode nullBody::errorCode() { return errorCode::expectedBodyNullBody; }