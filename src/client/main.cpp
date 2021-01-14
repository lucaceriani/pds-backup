#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

#include "Client.hpp"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

// Classe che gestisce la fase di connessione
class Connection{
public:
    Connection(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context): socket_(io_context, ssl_context){};
    // Connette il client al server
    void connect(const tcp::resolver::results_type& endpoints){
        boost::asio::async_connect(socket_.lowest_layer(), endpoints,
                                   [this](const boost::system::error_code& error,
                                          const tcp::endpoint& /*endpoint*/){
                                       if(!error){
                                           handshake();
                                       }else{
                                           std::cout << "Connection failed: " << error.message() << "\n";
                                       }
                                   });
    }
    // Handshake TLS
    void handshake(){
        socket_.async_handshake(boost::asio::ssl::stream_base::client,
                                [this](const boost::system::error_code& error){
                                    if(!error){
                                        std::cout << "Handshake completed! " << "\n";
                                        Client c = Client(std::move(socket_), folderToWatch);
                                        c.startClient();
                                    }else{
                                        std::cout << "Handshake failed: " << error.message() << "\n";
                                    }
                                });
    }
    // Setta la cartella da controllare
    void setFolder(std::string folder){
        folderToWatch = folder;
    }

private:
    ssl::stream<tcp::socket> socket_;
    std::string folderToWatch;
};

int main(int argc, char *argv[]) {

    try {
        if (argc != 4) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        ssl::context ssl_context(ssl::context::tlsv13);
        auto endpoints = resolver.resolve(argv[1], argv[2]);
        Connection conn(io_context, ssl_context);
        conn.setFolder(argv[3]);
        conn.connect(endpoints);

        io_context.run();

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    } catch (PDSBackup::BaseException &e) {
         std::cerr << "Errore server: " << e.what() << "\n";
    }
    return 0;
}