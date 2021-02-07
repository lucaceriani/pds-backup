#include <boost/asio.hpp>
#include <iostream>
#include <ctime>

#include "Client.hpp"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

int main(int argc, char *argv[]) {

    try {
        if (argc != 4) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        // Inizializzazione
        boost::asio::io_context io_context;
        ssl::context ssl_context(ssl::context::tlsv13);
        Client c = Client(io_context, ssl_context, {argv[1], argv[2]}, argv[3]);
        io_context.run();

    } catch (ClientExc::ClientException &c){
        std::cerr << "Errore client: " << c.what() << std::endl;
        return 1; // 1 = codice per terminazione causa eccezione del client
    } catch (PDSBackup::BaseException &e) {
        std::cerr << "Errore server: " << e.what() << "\n";
        time_t currentTime;
        time(&currentTime);
        std::cout << "Problema del server rilevato il: " << asctime(localtime(&currentTime));
        std::cout << "Il server termina." << std::endl;
        std::cout << "Eventuali modifiche della directory successive alla data e ora indicate non sono state salvate sul server." << std::endl;
        return 2; // 2 = codice per terminazione causa eccezioni PDSBackup (eccezioni server)
    }catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 3; // 3 = codice per terminazione causa eccezione di std::exception
    }

    return 0; // 0 = codice per terminazione normale del client
}