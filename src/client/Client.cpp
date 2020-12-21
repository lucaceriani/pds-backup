#include "Client.hpp"

Client::Client(tcp::socket s, std::string dir) : socket(std::move(s)), dirToWatch(std::move(dir)) {}

void Client::startClient() {
    // Richiesta di login al server
    Client::loginAsk();

    // Invio credenziali
    Client::loginAuthentication();

    // FileWatcher
    FileWatcher fw{dirToWatch, std::chrono::milliseconds(5000)};  // Crea e inizializza il FileWatcher

    // Fase di probe: si verifica che i file catalogati dal FileWatcher siano già presenti sul server, altrimenti si fa l'upload
    std::vector<std::string> toBeChecked = fw.getPaths_();

    for (std::string f : toBeChecked) {
        std::cout << f << std::endl;
        fileProbe(f);
    }

    // auto it = toBeChecked.begin();
    // while (it != toBeChecked.end()) {
    //     std::cout << *it << std::endl;
    // }

    // Avvio del FileWatcher: inizia a monitorare i cambiamenti della cartella lato client

    fw.start([this](std::string path_to_watch, FileStatus status) -> void {
        switch (status) {
            case FileStatus::created: {
                std::cout << "File creato sul client: " << path_to_watch << '\n';
                // Upload di un nuovo file sul server
                Client::fileUpload(path_to_watch, "Upload del nuovo file completato.");
                break;
            }
            case FileStatus::modified: {
                std::cout << "File modificato sul client: " << path_to_watch << '\n';
                // Upload di un file modificato sul server (come upload dei nuovi file ma il server sovrascrive)
                Client::fileUpload(path_to_watch, "Upload del file modificato completato.");
                break;
            }
            case FileStatus::erased: {
                std::cout << "File eliminato dal client: " << path_to_watch << '\n';
                // Eliminazione di un file dal server
                Client::fileDelete(path_to_watch);
                break;
            }
            case FileStatus::directoryCreated:
                std::cout << "Directory creata sul client: " << path_to_watch << '\n';
                // Nessuna comunicazione col server in questo caso
                break;
            case FileStatus::directoryErased: {
                std::cout << "Directory eliminata dal client: " << path_to_watch << '\n';
                // Eliminazione di un file dal server
                Client::directoryDelete(path_to_watch);
                break;
            }
            default:
                std::cout << "Errore! File status sconosciuto.\n";
        }
    });
}

void Client::loginAsk() {
    PDSBackup::MessageBuilder mb;
    while (true) {
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::loginRequest);
        message = mb.buildStr();
        boost::asio::write(socket, boost::asio::buffer(message, message.length()));
        // Controllo risposta server
        getAndSetRawHeader();
        if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok) {
            std::cout << "Login autorizzato." << std::endl;
            cu.reset();
            break;
        }
        cu.manageErrors();
        cu.reset();
    }
}

void Client::loginAuthentication() {
    PDSBackup::MessageBuilder mb;
    while (true) {
        std::string user;
        std::string pwd;
        std::cout << "Inserisci username: " << std::endl;
        std::cin >> user;
        std::cout << "Inserisci password: " << std::endl;
        std::cin >> pwd;
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::loginCredentials);
        mb.addField(user);
        mb.addField(pwd);
        message = mb.buildStr();
        boost::asio::write(socket, boost::asio::buffer(message, message.length()));
        // Controllo risposta server
        getAndSetRawHeader();
        if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok) {
            std::cout << "Login ok." << std::endl;
            sessionId = cu.getSessionId();
            cu.reset();

            break;
        }
        cu.manageErrors();
        cu.reset();
    }
}

void Client::fileProbe(std::string fileToCheck) {
    std::cout << "Faccio il probe di: " << fileToCheck << std::endl;

    PDSBackup::MessageBuilder mb;

    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileProbe);
    mb.setSessionId(sessionId);
    mb.addField(fileToCheck);
    mb.addField(PDSBackup::Checksum::md5(fileToCheck));
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    // Controllo risposta server
    getAndSetRawHeader();
    if (cu.getMessageCode() != PDSBackup::Protocol::MessageCode::ok) {
        if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::errorFileNotFound) {
            std::cout << "File non trovato nel server." << std::endl;
            // Il file non è presente sul server, lo carico
            cu.reset();
            Client::fileUpload(fileToCheck, "Upload del file mancante completato.");
        } else {
            cu.manageErrors();
        }
    }
    cu.reset();
}

void Client::fileUpload(std::string fileToUpload, std::string messageToPrint) {
    // Upload di un nuovo file sul server

    PDSBackup::MessageBuilder mb;

    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileUpload);
    mb.setSessionId(sessionId);
    mb.buildWithFile(fileToUpload, boost::filesystem::file_size(fileToUpload));
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));

    std::ifstream file;
    file.open(fileToUpload, std::ios::binary);
    char buff[8192];

    do {
        file.read(buff, 8192);
        boost::asio::write(socket, boost::asio::buffer(buff, file.gcount()));
    } while (!file.eof());

    file.close();

    // Controllo risposta server
    getAndSetRawHeader();
    if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << messageToPrint << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}

void Client::fileDelete(std::string fileToDelete) {
    // Eliminazione di un file dal server
    PDSBackup::MessageBuilder mb;
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileDelete);
    mb.setSessionId(sessionId);
    mb.addField(fileToDelete);
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    // Controllo risposta server
    getAndSetRawHeader();
    if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << "File eliminato correttamente dal server." << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}

void Client::directoryDelete(std::string directoryToDelete) {
    // Eliminazione di una cartella dal server
    PDSBackup::MessageBuilder mb;
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::folderDelete);
    mb.setSessionId(sessionId);
    mb.addField(directoryToDelete);
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    // Controllo risposta server
    getAndSetRawHeader();
    if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << "Directory elimiata correttamente dal server." << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}

void Client::getAndSetRawHeader() {
    // Legge l'header di risposta del server e lo fa elaborare da ClientUtility
    unsigned long long res = 0; // assumo inizialmente che non ci sia il body
    std::vector<char> rawHeader(PDSBackup::Protocol::headerLength);
    boost::system::error_code error;
    size_t len = boost::asio::read(socket, boost::asio::buffer(rawHeader), error);
    std::cout << std::string(rawHeader.begin(), rawHeader.end()) << std::endl;
    std::cout << "Letto header di lunghezza: " << len << std::endl;
    if (len != 0)
        res = cu.readHeader(rawHeader);
    else
        std::cout << "Header nullo!" << std::endl;
    if (res != 0)
        getAndSetRawBody(res);
}

void Client::getAndSetRawBody(unsigned long long bodyLen) {
    std::vector<char> rawBody(bodyLen);
    boost::system::error_code error;
    size_t len = boost::asio::read(socket, boost::asio::buffer(rawBody), error);
    std::cout << "Percorso file contenuto nel body: " << std::string(rawBody.begin(), rawBody.end()) << std::endl;
    std::cout << "Letto body di lunghezza: " << len << std::endl;
    if(len != 0)
        cu.readBody(rawBody);
    else
        std::cout << "Body nullo!" << std::endl;
}