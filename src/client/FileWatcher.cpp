#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <tuple>

#include "FileWatcher.hpp"

FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
    for (auto &file : boost::filesystem::recursive_directory_iterator(boost::filesystem::path(path_to_watch))) {
        if(boost::filesystem::is_directory(file.path()))
            paths_[file.path().string()] = std::make_tuple(boost::filesystem::last_write_time(file), "directory");
        else
            paths_[file.path().string()] = std::make_tuple(boost::filesystem::last_write_time(file), "file");
    }
}

void FileWatcher::start(const std::function<void(std::string, FileStatus)> &action) {
    while (running_) {
        // Attende per un tempo specificato da "delay" e poi verifica eventuali modifiche nella directory indicata
        std::this_thread::sleep_for(delay);

        // File eliminato
        auto it = paths_.begin();
        while (it != paths_.end()) {
            if (!boost::filesystem::exists(it->first)) {
                if(std::get<1>(it->second) == "directory"){
                    action(it->first, FileStatus::directoryErased);
                    it = paths_.erase(it);
                }else{
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
                if(boost::filesystem::is_directory(file.path())){
                    // Directory creata
                    paths_[file.path().string()] = std::make_tuple(current_file_last_write_time, "directory");
                    action(file.path().string(), FileStatus::directoryCreated);
                }else{
                    // File creato
                    paths_[file.path().string()] = std::make_tuple(current_file_last_write_time, "file");
                    action(file.path().string(), FileStatus::created);
                }
            } else {
                // File modificato (al momento se modifico con editor grafici tutto ok, se uso "nano" da terminale da un po' di problemi, crea un file .swp) ;
                if (std::get<0>(paths_[file.path().string()]) != current_file_last_write_time && !boost::filesystem::is_directory(file.path())) { //faccio in modo che venga aggiornato il tempo dell' istante di modifica solo nel caso di modifica dei file ;
                    paths_[file.path().string()] = std::make_tuple(current_file_last_write_time, "file");
                    action(file.path().string(), FileStatus::modified);
                }
            }
        }
    }
}

std::vector<std::string> FileWatcher::getPaths_(){
    std::vector<std::string> tmp;
    auto it = paths_.begin();
    while (it != paths_.end()) {
        if(std::get<1>(it->second) == "directory"){
            tmp.push_back(it->first);
        }
    }
    return tmp;
}

bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}