#pragma once

#include <boost/filesystem.hpp>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

//Status del FileWatcher
enum class FileStatus {created, modified, erased};


class FileWatcher {
public:
    boost::filesystem::path path_to_watch; // Cartella da controllare
    std::chrono::duration<int, std::milli> delay; // Intervallo di tempo per il check della cartella

    // Creo una struttura dati ("paths_") con i file e le informazioni sulla loro ultima modifica
    FileWatcher(boost::filesystem::path path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
        for(auto &file : boost::filesystem::recursive_directory_iterator(path_to_watch)) {
            paths_[file.path().string()] = boost::filesystem::last_write_time(file);
        }
    }

    // Monitora path_to_watch e in caso di eventuali modifiche esegue una funzione
    void start(const std::function<void (boost::filesystem::path, FileStatus)> &action) {
        while(running_) {
            // Attende per un tempo specificato da "delay" e poi verifica eventuali modifiche nella directory indicata
            std::this_thread::sleep_for(delay);

            //File eliminato
            auto it = paths_.begin();
            while (it != paths_.end()) {
                if (!boost::filesystem::exists(it->first)) {
                    action(it->first, FileStatus::erased);
                    it = paths_.erase(it);
                }
                else {
                    it++;
                }
            }
            // N.B.: per gestire il renaming sarebbe necessario usare Inotify (ma va solo in ambiente Linux) oppure
            // gestire una tabella di hash ma rallenterebbe molto il programma (fonte: gruppo Telegram); in ogni caso
            // sul testo del progetto si parla solo di add/erase/change e non di rename quindi non dovrebbe essere
            // un controllo richiesto dalle specifiche;

            // Controlla se un file è stato creato o modificato
            for(auto &file : boost::filesystem::recursive_directory_iterator(path_to_watch)) {
                auto current_file_last_write_time = boost::filesystem::last_write_time(file);

                // File creato
                if(!contains(file.path().string())) {
                    paths_[file.path().string()] = current_file_last_write_time;
                    action(file.path().string(), FileStatus::created);
                    // File modificato
                } else {
                    if(paths_[file.path().string()] != current_file_last_write_time) {
                        paths_[file.path().string()] = current_file_last_write_time;
                        action(file.path().string(), FileStatus::modified);
                    }
                }
            }
        }
    }

private:
    std::unordered_map<std::string, std::time_t> paths_;
    bool running_ = true;

    // In teoria sostituibile in c++20 con la funzione di libreria ".contains()"
    bool contains(const std::string &key) {
        auto el = paths_.find(key);
        return el != paths_.end();
    }
};