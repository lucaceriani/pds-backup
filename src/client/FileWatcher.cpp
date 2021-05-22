#include "FileWatcher.hpp"

#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>

// Costruttore: inizializza le strutture dati opportune per il funzionamento del FileWatcher.

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
    for (auto &file : boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path_to_watch))) {
        if (boost::filesystem::is_directory(file.path()))
            paths_[file.path().string()] = std::make_tuple(boost::filesystem::last_write_time(file), "directory");
        else
            paths_[file.path().string()] = std::make_tuple(boost::filesystem::last_write_time(file), "file");
    }
}

// Funzione che avvia il FileWatcher ed esegue un controllo su creazione/modifica/eliminazione di file e cartelle
// con cadenza regolare, seguendo un intervallo di tempo specifico.

void FileWatcher::start(const std::function<void(std::string, FileStatus)> &action) {
    while (running_) {
        // Attende per un tempo specificato da "delay" e poi verifica eventuali modifiche nella directory indicata
        std::this_thread::sleep_for(delay);

        // File eliminato
        auto it = paths_.begin();
        while (it != paths_.end()) {
            if (!boost::filesystem::exists(it->first)) {
                if (std::get<1>(it->second) == "directory") {
                    action(it->first, FileStatus::directoryErased);
                    it = paths_.erase(it);
                } else {
                    action(it->first, FileStatus::erased);
                    it = paths_.erase(it);
                }
            } else {
                it++;
            }
        }

        // Controlla se un file è stato creato o modificato
        for (auto &file : boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path_to_watch))) {
            auto current_file_last_write_time = boost::filesystem::last_write_time(file);

            if (!contains(file.path().string())) {
                if (boost::filesystem::is_directory(file.path())) {
                    // Directory creata
                    paths_[file.path().string()] = std::make_tuple(current_file_last_write_time, "directory");
                    action(file.path().string(), FileStatus::directoryCreated);
                } else {
                    // File creato
                    paths_[file.path().string()] = std::make_tuple(current_file_last_write_time, "file");
                    action(file.path().string(), FileStatus::created);
                }
            } else {
                // File modificato;
                if (std::get<0>(paths_[file.path().string()]) != current_file_last_write_time && !boost::filesystem::is_directory(file.path())) {  //faccio in modo che venga aggiornato il tempo dell' istante di modifica solo nel caso di modifica dei file ;
                    paths_[file.path().string()] = std::make_tuple(current_file_last_write_time, "file");
                    action(file.path().string(), FileStatus::modified);
                }
            }
        }
    }
}

// Funzione che fa da getter per i file che sono salvati al momento nella struttura dati "paths_" (unordered map).

std::vector<std::string> FileWatcher::getPaths_() {
    std::vector<std::string> tmp;
    for (auto it = paths_.begin(); it != paths_.end(); it++) {
        if (std::get<1>(it->second) == "file") {
            tmp.push_back(it->first);
        }
    }
    return tmp;
}

// Funzione che controlla se un file è contenuto in "paths_" o meno e restituisce un valore booleano.

bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}