#include <boost/asio.hpp>
#include <iostream>

#include "Client.hpp"

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {

    try {
        if (argc != 4) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

        Client c = Client(std::move(s), argv[3]);

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    } catch (PDSBackup::BaseException &e) {
         std::cerr << "Errore server: " << e.what() << "\n";
    }
    return 0;
}