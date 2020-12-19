#include "Client.hpp"

Client::Client(tcp::socket s, std::string dir) : socket(std::move(s)), dirToWatch(std::move(dir)){}

void Client::startClient(){

// Richiesta di login al server
    Client::loginAsk();

// Invio credenziali
    Client::loginAuthentication();

// FileWatcher
    FileWatcher fw{dirToWatch, std::chrono::milliseconds(5000)}; // Crea e inizializza il FileWatcher

// Fase di probe: si verifica che i file catalogati dal FileWatcher siano già presenti sul server, altrimenti si fa l'upload
    std::vector<std::string> toBeChecked = fw.getPaths_();
    auto it = toBeChecked.begin();
    while(it != toBeChecked.end()){
        fileProbe(*it);
    }

// Avvio del FileWatcher: inizia a monitorare i cambiamenti della cartella lato client

    fw.start([this] (std::string path_to_watch, FileStatus status) -> void {

        switch(status) {
            case FileStatus::created: {
                std::cout << "File created: " << path_to_watch << '\n';
                // Upload di un nuovo file sul server
                Client::fileUpload(path_to_watch, "New file upload done.");
                break;
            }
            case FileStatus::modified:{
                std::cout << "File modified: " << path_to_watch << '\n';
                // Upload di un file modificato sul server (come upload dei nuovi file ma il server sovrascrive)
                Client::fileUpload(path_to_watch, "Modified file upload done.");
                break;
            }
            case FileStatus::erased: {
                std::cout << "File erased: " << path_to_watch << '\n';
                // Eliminazione di un file dal server
                Client::fileDelete(path_to_watch);
                break;
            }
            case FileStatus::directoryCreated:
                std::cout << "Directory created: " << path_to_watch << '\n';
                // Nessuna comunicazione col server in questo caso
                break;
            case FileStatus::directoryErased: {
                std::cout << "Directory erased: " << path_to_watch << '\n';
                // Eliminazione di un file dal server
                Client::directoryDelete(path_to_watch);
                break;
            }
            default:
                std::cout << "Error! Unknown file status.\n";
        }

    });
}


void Client::loginAsk(){
    while(true){
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::loginRequest);
        message = mb.buildStr();
        boost::asio::write(socket, boost::asio::buffer(message, message.length()));
        // Controllo risposta server
        getAndSetRawHeader();
        if(cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok){
            std::cout << "Login authorized." << std::endl;
            cu.reset();
            break;
        }
        cu.manageErrors();
        cu.reset();
    }
}


void Client::loginAuthentication(){
    while(true){
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
        if(cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok){
            std::cout << "Login ok." << std::endl;
            sessionId = cu.getSessionId();
            cu.reset();
            break;
        }
        cu.manageErrors();
        cu.reset();
    }
}

void Client::fileProbe(std::string fileToCheck){
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileProbe);
    mb.setSessionId(sessionId);
    mb.addField(fileToCheck);
    mb.addField(PDSBackup::Checksum::md5(fileToCheck));
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    // Controllo risposta server
    getAndSetRawHeader();
    if(cu.getMessageCode() != PDSBackup::Protocol::MessageCode::ok){
        if(cu.getMessageCode() == PDSBackup::Protocol::MessageCode::errorFileNotFound){
            std::cout << "File not found in server." << std::endl;
            // Il file non è presente sul server, lo carico
            Client::fileUpload(fileToCheck, "Missing file upload done.");
        }else
            cu.manageErrors();
    }
    cu.reset();
}

void Client::fileUpload(std::string fileToUpload, std::string messageToPrint){
    // Upload di un nuovo file sul server
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileUpload);
    mb.setSessionId(sessionId);
    mb.buildWithFile(fileToUpload, boost::filesystem::file_size(fileToUpload));
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    std::ifstream file;
    file.open(fileToUpload, std::ios::binary);
    char buff[8192];
    while (file.read(buff, 8192)) {
        boost::asio::write(socket, boost::asio::buffer(buff, file.gcount()));
    }
    file.close();
    // Controllo risposta server
    getAndSetRawHeader();
    if(cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << messageToPrint << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}

void Client::fileDelete(std::string fileToDelete){
    // Eliminazione di un file dal server
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileDelete);
    mb.setSessionId(sessionId);
    mb.addField(fileToDelete);
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    // Controllo risposta server
    getAndSetRawHeader();
    if(cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << "File correctly erased from the server." << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}


void Client::directoryDelete(std::string directoryToDelete){
    // Eliminazione di una cartella dal server
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::folderDelete);
    mb.setSessionId(sessionId);
    mb.addField(directoryToDelete);
    message = mb.buildStr();
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    // Controllo risposta server
    getAndSetRawHeader();
    if(cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << "Directory correctly erased from the server." << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}

void Client::getAndSetRawHeader(){
    // Legge l'header di risposta del server e lo fa elaborare da ClientUtility
    std::vector<char> rawHeader;
    boost::system::error_code error;
    size_t len = socket.read_some(boost::asio::buffer(rawHeader, PDSBackup::Protocol::headerLength), error);
    std::cout << "Letto header di lunghezza: " << len << std::endl;
    cu.readHeader(rawHeader);
}