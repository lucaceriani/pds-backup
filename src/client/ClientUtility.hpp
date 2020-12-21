#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "../shared/_include.hpp"

using boost::asio::ip::tcp;

class ClientUtility{
public:
    ClientUtility();
    unsigned long long readHeader(std::vector<char> rawHeader);
    void readBody(std::vector<char> rawBody);
    PDSBackup::Protocol::MessageCode getMessageCode();
    std::string printLen(std::vector<char> s);
    void reset();
    void manageErrors();
    std::string getSessionId();
private:
    PDSBackup::Header header;
    PDSBackup::Body body;
    std::string path;
};