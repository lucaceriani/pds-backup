#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "../shared/_include.hpp"
#include "ClientUtility.hpp"
#include "FileWatcher.hpp"


using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

class Client {
   public:
    Client(ssl::stream<tcp::socket> s, std::string dir);
    void startClient();

   private:
    ssl::stream<tcp::socket> socket;
    std::string message;
    std::string sessionId;
    std::string dirToWatch;
    ClientUtility cu;

    void loginAsk();
    void loginAuthentication();
    void fileProbe(std::string fileToCheck);
    void fileUpload(std::string fileToUpload, std::string messageToPrint);
    void fileDelete(std::string fileToDelete);
    void directoryDelete(std::string directoryToDelete);
    void getAndSetRawHeader();
    void getAndSetRawBody(unsigned long long bodyLen);
};