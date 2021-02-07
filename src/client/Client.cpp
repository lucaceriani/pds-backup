#include "Client.hpp"

Client::Client(boost::asio::io_context& io_context,
               boost::asio::ssl::context& ssl_context,
               boost::asio::ip::tcp::resolver::query que,
               std::string dir) : socket(io_context, ssl_context), query(que) {
    dirToWatch = std::move(dir);
    connect();
}

void Client::startClient() {
    // Richiesta di login al server
    Client::loginAsk();
    loginAcceptedFlag = 0; //Effettuato il login riazzero il flag in modo da poterlo riutilizzare in caso di fail del server per ritentare il login

    // Invio credenziali
    Client::loginAuthentication();

    // Setta il flag alreadyStarted
    alreadyStarted = true;

    // Crea e inizializza il FileWatcher
    FileWatcher fw{dirToWatch, std::chrono::milliseconds(5000)};

    // Fase di probe: si verifica che i file catalogati dal FileWatcher siano già presenti sul server, altrimenti si fa l'upload
    std::vector<std::string> toBeChecked = fw.getPaths_();
    for (std::string f : toBeChecked) {
        std::cout << f << std::endl;
        fileProbe(f);
    }

    // Avvio del FileWatcher: inizia a monitorare i cambiamenti della cartella lato client
    fw.start([this](std::string path_to_watch, FileStatus status) -> void {
        switch (status) {
            case FileStatus::created: {
                std::cout << "File creato sul client: " << path_to_watch << '\n';
                // Upload di un nuovo file sul server
                Client::fileUpload(path_to_watch, "Upload del nuovo file completato.");
                if(restartDone){
                    fileProbe(path_to_watch);
                    restartDone = false; // Risetto il flag per eventuali future disconnessioni e riprendo il flusso
                }
                break;
            }
            case FileStatus::modified: {
                std::cout << "File modificato sul client: " << path_to_watch << '\n';
                // Upload di un file modificato sul server (come upload dei nuovi file ma il server sovrascrive)
                Client::fileUpload(path_to_watch, "Upload del file modificato completato.");
                // Faccio il probe del file che stavo inviando in modo che venga aggiornato sul server
                // (altrimenti quando il client riparte si accorge solo dei file creati/eliminati e non delle modifiche)
                if(restartDone){
                    fileProbe(path_to_watch);
                    restartDone = false; // Risetto il flag per eventuali future disconnessioni e riprendo il flusso
                }
                break;
            }
            case FileStatus::erased: {
                std::cout << "File eliminato dal client: " << path_to_watch << '\n';
                // Eliminazione di un file dal server
                Client::fileDelete(path_to_watch);
                if(restartDone){
                    Client::fileDelete(path_to_watch);
                    restartDone = false; // Risetto il flag per eventuali future disconnessioni e riprendo il flusso
                }
                break;
            }
            case FileStatus::directoryCreated:
                std::cout << "Directory creata sul client: " << path_to_watch << '\n';
                // Nessuna comunicazione col server in questo caso
                break;
            case FileStatus::directoryErased: {
                std::cout << "Directory eliminata dal client: " << path_to_watch << '\n';
                // Eliminazione di una cartella dal server
                Client::directoryDelete(path_to_watch);
                if(restartDone){
                    Client::directoryDelete(path_to_watch);
                    restartDone = false; // Risetto il flag per eventuali future disconnessioni e riprendo il flusso
                }
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
        try{
            boost::asio::write(socket, boost::asio::buffer(message, message.length()));
            getAndSetRawHeader();
        } catch(ClientExc::ClientException &c){
            if(c.errorCode() == ClientExc::errorCode::expectedBodyNullBody){
                throw ClientExc::nullBody();
            }
            if(c.errorCode() == ClientExc::errorCode::nullHeaderConnectionFailed){
                std::cout << "Eccezione client: " << c.what() << std::endl;
                std::cout << "Problemi di connessione..." << std::endl;
                Client::waitAndReconnect();
                return;
            }
        }
        // Controllo risposta server
        if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok) {
            loginAcceptedFlag = 1;
            std::cout << "Login autorizzato." << std::endl;
            cu.reset();
            break;
        }
        mb.clearFields();
        cu.manageErrors();
        cu.reset();
    }
}

void Client::loginAuthentication() {
    PDSBackup::MessageBuilder mb;
    while (true) {
        std::string user;
        std::string pwd;
        if(hiddenLoginFlag == 0){
            std::cout << "Inserisci username: " << std::endl;
            std::cin >> user;
            std::cout << "Inserisci password: " << std::endl;
            std::cin >> pwd;
        }else{
            user = userMem;
            pwd = pwdMem;
        }
        mb.setMessageCode(PDSBackup::Protocol::MessageCode::loginCredentials);
        mb.addField(user);
        mb.addField(pwd);
        message = mb.buildStr();
        try{
            boost::asio::write(socket, boost::asio::buffer(message, message.length()));
            getAndSetRawHeader();
        } catch(ClientExc::ClientException &c){
            if(c.errorCode() == ClientExc::errorCode::expectedBodyNullBody){
                throw ClientExc::nullBody();
            }
            if(c.errorCode() == ClientExc::errorCode::nullHeaderConnectionFailed){
                std::cout << "Eccezione client: " << c.what() << std::endl;
                std::cout << "Problemi di connessione..." << std::endl;
                Client::waitAndReconnect();
                return;
            }
        }
        // Controllo risposta server
        if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok) {
            std::cout << "Login ok." << std::endl;
            // Salva in memoria user e pwd per potersi riconnettere in caso di fail del server
            userMem = user;
            pwdMem = pwd;
            hiddenLoginFlag = 1;
            sessionId = cu.getSessionId();
            cu.reset();
            break;
        }
        mb.clearFields();
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
    try{
        boost::asio::write(socket, boost::asio::buffer(message, message.length()));
        getAndSetRawHeader();
    } catch(ClientExc::ClientException &c){
        if(c.errorCode() == ClientExc::errorCode::expectedBodyNullBody){
            throw ClientExc::nullBody();
        }
        if(c.errorCode() == ClientExc::errorCode::nullHeaderConnectionFailed){
            std::cout << "Eccezione client: " << c.what() << std::endl;
            std::cout << "Problemi di connessione..." << std::endl;
            Client::waitAndReconnect();
            return;
        }
    }
    // Controllo risposta server
    if (cu.getMessageCode() != PDSBackup::Protocol::MessageCode::ok) {
        if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::errorFileNotFound) {
            std::cout << "File non trovato sul server." << std::endl;
            // Il file non è presente sul server quindi lo carica
            cu.reset();
            Client::fileUpload(fileToCheck, "Upload del file mancante o non aggiornato completato.");
        } else {
            cu.manageErrors();
        }
    }
    cu.reset();
}

void Client::fileUpload(std::string fileToUpload, std::string messageToPrint) {
    // Upload di un nuovo file sul server
    PDSBackup::MessageBuilder mb;
    boost::system::error_code error;
    mb.setMessageCode(PDSBackup::Protocol::MessageCode::fileUpload);
    mb.setSessionId(sessionId);
    mb.buildWithFile(fileToUpload, boost::filesystem::file_size(fileToUpload));
    message = mb.buildStr();
    try{
        boost::asio::write(socket, boost::asio::buffer(message, message.length()));
        std::ifstream file;
        file.open(fileToUpload, std::ios::binary);
        char buff[8192];
        do {
            file.read(buff, PDSBackup::Protocol::bufferSize);
            boost::asio::write(socket, boost::asio::buffer(buff, file.gcount()));
        } while (!file.eof());
        file.close();
        getAndSetRawHeader();
    } catch(ClientExc::ClientException &c){
        if(c.errorCode() == ClientExc::errorCode::expectedBodyNullBody){
            throw ClientExc::nullBody();
        }
        if(c.errorCode() == ClientExc::errorCode::nullHeaderConnectionFailed){
            std::cout << "Eccezione client: " << c.what() << std::endl;
            std::cout << "Problemi di connessione..." << std::endl;
            Client::waitAndReconnect();
            return;
        }
    } catch(std::exception &e){
        std::cout << "Eccezione client: " << e.what() << std::endl;
        std::cout << "Problemi di connessione..." << std::endl;
        Client::waitAndReconnect();
        return;
    }
    // Controllo risposta server
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
    try{
    boost::asio::write(socket, boost::asio::buffer(message, message.length()));
    getAndSetRawHeader();
    } catch(ClientExc::ClientException &c){
        if(c.errorCode() == ClientExc::errorCode::expectedBodyNullBody){
            throw ClientExc::nullBody();
        }
        if(c.errorCode() == ClientExc::errorCode::nullHeaderConnectionFailed){
            std::cout << "Eccezione client: " << c.what() << std::endl;
            std::cout << "Problemi di connessione..." << std::endl;
            Client::waitAndReconnect();
            return;
        }
    }
    // Controllo risposta server
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
    try {
        boost::asio::write(socket, boost::asio::buffer(message, message.length()));
        getAndSetRawHeader();
    } catch(ClientExc::ClientException &c){
        if(c.errorCode() == ClientExc::errorCode::expectedBodyNullBody){
            throw ClientExc::nullBody();
        }
        if(c.errorCode() == ClientExc::errorCode::nullHeaderConnectionFailed){
            std::cout << "Eccezione client: " << c.what() << std::endl;
            std::cout << "Problemi di connessione..." << std::endl;
            Client::waitAndReconnect();
            return;
        }
    }
    // Controllo risposta server
    if (cu.getMessageCode() == PDSBackup::Protocol::MessageCode::ok)
        std::cout << "Directory elimiata correttamente dal server." << std::endl;
    else
        cu.manageErrors();
    cu.reset();
}

void Client::getAndSetRawHeader() {
    // Legge l'header di risposta del server e lo fa elaborare da ClientUtility
    unsigned long long res; // assumo inizialmente che non ci sia il body
    std::vector<char> rawHeader(PDSBackup::Protocol::headerLength);
    boost::system::error_code error;
    size_t len = boost::asio::read(socket, boost::asio::buffer(rawHeader), error);
    std::cout << std::string(rawHeader.begin(), rawHeader.end()) << std::endl;
    std::cout << "Letto header di lunghezza: " << len << std::endl;
    if (len != 0)
        res = cu.readHeader(rawHeader);
    else{
        std::cout << "Header nullo!" << std::endl;
        throw ClientExc::connectionFailed();
    }

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
    else {
        std::cout << "Body nullo!" << std::endl;
        throw ClientExc::nullBody();
    }
}

void Client::connect(){
    boost::asio::io_context& io_context = static_cast<boost::asio::io_context&>(socket.get_executor().context());
    tcp::resolver r (io_context);
    boost::asio::async_connect(socket.lowest_layer(), r.resolve(query),
                               [this](const boost::system::error_code& error,
                                      const tcp::endpoint& /*endpoint*/){
                                   if(!error){
                                       handshake();
                                   }else{
                                       std::cout << "Connessione fallita: " << error.message() << "\n";
                                   }
                               });
}

void Client::handshake(){
    socket.async_handshake(boost::asio::ssl::stream_base::client,
                            [this](const boost::system::error_code& error){
                                if(!error){
                                    std::cout << "Handshake completato! " << "\n";
                                    if(alreadyStarted == false){
                                        startClient();
                                    }else{
                                        restartClientAfterFail();
                                        restartDone = true;
                                    }
                                }else{
                                    std::cout << "Handshake fallito: " << error.message() << "\n";
                                }
                            });
}

void Client::waitAndReconnect(){
    // Salvo l' orario in cui mi accorgo che il server è down
    time_t currentTime;
    time(&currentTime);
    int attemptCounter = 0; // Conta quante volte ho tentato la riconnessione (es. qui dopo 600 volte = 10 minuti, termina il client definitivamente)
    boost::asio::io_context& io_context = static_cast<boost::asio::io_context&>(socket.get_executor().context());
    bool waitingFlag = true;
    while(waitingFlag){
        attemptCounter++;
        std::cout << "Tentativo di riconnessione al server numero " << attemptCounter << "..." << std::endl;
        SSL_clear(socket.native_handle());
        connect();
        io_context.run_for(std::chrono::milliseconds(1000)); // Ogni secondo controllo se il server è tornato disponibile
        if(restartDone)
            waitingFlag = false;
        if(attemptCounter == 600){
            std::cout << "Tempo limite superato, non è riuscita la riconnessione." << std::endl;
            // Prima di terminare stampo l' orario a cui il server ha terminato di funzionare
            // in modo da permettere all' utente di verificare se il suo backup è completo considerando l' orario fino a cui
            // il server ha funzionato regolarmente.
            std::cout << "Problema di connessione rilevato il: " << asctime(localtime(&currentTime));
            std::cout << "Eventuali modifiche della directory successive alla data e ora indicate non sono state salvate sul server." << std::endl;
            throw ClientExc::timeout();
        }
    }
    std::cout << "Riconnessione con il server effettuata, riprendo il monitoraggio." << std::endl;
    return;
}

void Client::restartClientAfterFail(){
    // Richiesta di login al server
    Client::loginAsk();
    loginAcceptedFlag = 0; //Effettuato il login riazzero il flag in modo da poterlo riutilizzare in caso di fail del server per ritentare il login
    // Invio credenziali: utilizzo user e pwd memorizzati al primo accesso
    Client::loginAuthentication();
}