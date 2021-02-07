#pragma once

#include <boost/filesystem.hpp>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <tuple>

//Status del FileWatcher
enum class FileStatus {created, modified, erased, directoryCreated, directoryErased};

class FileWatcher {
public:
    std::string path_to_watch; // Cartella da controllare
    std::chrono::duration<int, std::milli> delay; // Intervallo di tempo per il check della cartella
    FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay); // Creo una struttura dati ("paths_") con i file e le informazioni sulla loro ultima modifica
    void start(const std::function<void(std::string, FileStatus)> &action);  // Monitora path_to_watch e in caso di eventuali modifiche esegue una funzione
    std::vector<std::string> getPaths_();  // Getter per i path, serve per la fase di probe (considera solo i file e non le cartelle)
private:
    std::unordered_map<std::string, std::tuple<std::time_t, std::string>> paths_; // Memorizza file e cartelle presenti nella directory da monitorare
    bool running_ = true;
    bool contains(const std::string &key);
};