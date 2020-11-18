#ifndef PDS_BACKUP_SHARED_PROTOCOL
#define PDS_BACKUP_SHARED_PROTOCOL

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace PDSBackup
{

    class Protocol
    {
    private:
        std::string header;
        tcp::socket socket;

    public:
        Protocol(tcp::socket s) : socket(std::move(s)) {}

        // Legge l'header dal socket e lo mette dentro header
        bool readHeader();
        std::string stringHeader();

        enum class Const
        {
            headerLenght = 40,
            sepChar = 0,
        };
    };

} // namespace PDSBackup

#endif
