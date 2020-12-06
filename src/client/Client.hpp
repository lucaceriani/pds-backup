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

class Client{
public:
    Client(tcp::socket s);
    void readHeader();
    void readBody();
    PDSBackup::Protocol::MessageCode getMessageCode();
    std::string printLen(std::vector<char> s);
    void reset();
    void manageErrors();
    std::string getSessionId();
private:
    boost::asio::ip::tcp::socket socket;
    std::vector<char> rawHeader;
    std::vector<char> rawBody;
    PDSBackup::Header header;
    PDSBackup::Body body;
    std::string path;
    unsigned long long bodyReadSoFar;
};