#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

#include "../shared/_include.hpp"
#include "ClientUtility.hpp"
#include "FileWatcher.hpp"
#include "ClientExceptions.hpp"


using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

class Client {
   public:
    Client(boost::asio::io_context& io_context, boost::asio::ssl::context& context, boost::asio::ip::tcp::resolver::query que, std::string dir);
    void startClient();

   private:
    ssl::stream<tcp::socket> socket;
    std::string message;
    std::string sessionId;
    std::string dirToWatch;
    ClientUtility cu;
    std::string userMem;
    std::string pwdMem;
    int loginAcceptedFlag = 0;
    int hiddenLoginFlag = 0;
    boost::asio::ip::tcp::resolver::query query;
    bool alreadyStarted = false;
    bool restartDone = false;

    void loginAsk();
    void loginAuthentication();
    void fileProbe(std::string fileToCheck);
    void fileUpload(std::string fileToUpload, std::string messageToPrint);
    void fileDelete(std::string fileToDelete);
    void directoryDelete(std::string directoryToDelete);
    void getAndSetRawHeader();
    void getAndSetRawBody(unsigned long long bodyLen);
    void handshake();
    void connect();
    void waitAndReconnect();
    void restartClientAfterFail();
};