#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "../shared/_include.hpp"

class Client{
public:
    Client(tcp::socket socket);
    void readHeader();
    void readBody();
    std::string printLen(std::vector<char> s);
private:
    tcp::socket socket;
    std::ofstream stream;
    std::vector<char> rawHeader;
    std::vector<char> rawBody;
    PDSBackup::Header header;
    PDSBackup::Body body;
    unsigned long long bodyReadSoFar;
};