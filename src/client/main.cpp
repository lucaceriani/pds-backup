#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../shared/Checksum.hpp"

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    try {
        // if (argc != 3) {
        //     std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
        //     return 1;
        // }

        // std::cout << PDSBackup::Checksum::md5("00__test.jpg") << std::endl;
        // return 0;
        /*
        boost::asio::io_service io_service;

        // tcp::iostream sockstream(tcp::resolver::query{argv[1], argv[2]});
        tcp::iostream sockstream(tcp::resolver::query{"localhost", "1234"});

        std::ifstream ifs("./__test.jpg", std::ios::binary);

        std::string msg = "Questo e' il messaggio che vogio inviare";
        std::string msg2 = "Il secondo campo e' anche piu' lungo del primo";

        sockstream << "0001M021";
        // sockstream.flush();
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        sockstream << "aaaabbbbccccddddeeeeffff";
        sockstream << std::setfill('0') << std::setw(16) << boost::filesystem::file_size("./__test.jpg");
        sockstream << "__ricevuto.jpg";
        sockstream << '\0';
        sockstream << ifs.rdbuf();
        // sockstream << std::setfill('0') << std::setw(16) << msg.length() + msg2.length() + 1;
        // sockstream << msg << '\0' << msg2;
        sockstream.flush();
*/
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "localhost", "1234");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        std::string request;
        tcp::socket s(io_service);
        s.connect(*iterator);

        // std::cout << "Enter message: ";
        // char request[max_length];
        request = "0001M010________________________0000000000000013Ciao come va?";
        boost::asio::write(s, boost::asio::buffer(request.data(), request.length()));

        // esempio invio file
        request = "0001M021______mess con file_____0000000000120393fileBelissimo.jpg";
        request += '\0';
        boost::asio::write(s, boost::asio::buffer(request, request.length()));
        std::ifstream fp;
        fp.open("__test.jpg", std::ios::binary);
        char b[1024];
        while (42) {
            fp.read(b, 1024);
            boost::asio::write(s, boost::asio::buffer(b, fp.gcount()));
            if (fp.eof()) break;
        }
        fp.close();

        // esempio invio altro messaggio senza file
        request = "0001M010______secondo mess______0000000000000013Inseriscodati";
        boost::asio::write(s, boost::asio::buffer(request.data(), request.length()));

        // std::cout.write(request.data(), 50);
        // std::cout << std::endl;

        // std::this_thread::sleep_for(std::chrono::seconds(2));

        // boost::asio::write(s, boost::asio::buffer(request.data() + 50, request.length() - 50));
        // std::cout.write(request.data() + 50, request.length() - 50);
        // std::cout << std::endl;

        s.close();

        // char reply[1024];
        // //size_t reply_length = s.read_some(boost::asio::buffer(reply));
        // sockstream >> reply;
        // std::cout << "Reply is: " << reply << std::endl;

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}