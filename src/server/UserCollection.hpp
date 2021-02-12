#pragma once

#include <map>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "User.hpp"

namespace PDSBackup {

class UserCollection {
   public:
    void add(User user, bool isAtomic = true);

    std::optional<std::string> login(std::string name, std::string pass, bool isAtomic = true);
    std::optional<SessionId> getSessionId(std::string sid, bool isAtomic = true);

   private:
    std::shared_mutex sm;
    std::unordered_map<std::string, SessionId> sessionIdMap;
    std::unordered_map<std::string, User> userByName;

    std::optional<User> getUserByName(std::string uname, bool isAtomic = true);
};

}  // namespace PDSBackup