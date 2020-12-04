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
private:
    std::unordered_map<std::string, std::tuple<std::time_t, std::string>> paths_; //sono arrivato alla conclusione che per distinguere file e cartelle nel caso dell' eliminazione devo modificare il modo in cui le memorizzo aggiungendo informazioni sul loro tipo (file o cartelle)
    bool running_ = true;
    bool contains(const std::string &key); // In teoria sostituibile in C++20 con la funzione di libreria ".contains()"
};