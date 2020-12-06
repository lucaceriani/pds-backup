#pragma once

#include <map>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

#include "User.hpp"

namespace PDSBackup {

class UserCollection {
   public:
    void add(User user, bool isAtomic = true);

    std::optional<std::string> login(std::string name, std::string pass, bool isAtomic = true);

   private:
    std::mutex m;
    std::unordered_map<std::string, std::string> sidByName;
    std::unordered_map<std::string, User> userByName;

    std::optional<User> getUserByName(std::string uname, bool isAtomic = true);
    std::optional<std::string> getSidByName(std::string uname, bool isAtomic = true);
};

}  // namespace PDSBackup