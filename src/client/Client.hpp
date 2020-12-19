#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "FileWatcher.hpp"
#include "ClientUtility.hpp"
#include "../shared/_include.hpp"

using boost::asio::ip::tcp;

class Client{
public:
    Client(tcp::socket s, std::string dir);
    void startClient();

private:
    tcp::socket socket;
    PDSBackup::MessageBuilder mb;
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
};