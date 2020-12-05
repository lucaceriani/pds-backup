#include "UserCollection.hpp"

#include <optional>

using namespace PDSBackup;

std::optional<std::string> UserCollection::login(std::string username, std::string pass, bool isAtomic) {
    if (isAtomic) std::lock_guard _(m);  // cpp17

    auto optUser = getUserByName(username);
    if (optUser.has_value()) {
        // se ho trovasto l'utente provo a fare il login
        User user = optUser.value();
        auto optSid = user.login(username, pass);
        if (optSid.has_value()) {
            // TODO magari fare il controllo sulle sid duplicate

            sessionIdMap[optSid.value().sessionId] = optSid.value();
            return optSid.value().sessionId;
        }
    }

    return std::nullopt;
}

std::optional<User> UserCollection::getUserByName(std::string uname, bool isAtomic) {
    if (isAtomic) std::lock_guard _(m);  // cpp17

    auto user = userByName.find(uname);
    if (user == userByName.end()) {
        return std::nullopt;
    } else {
        return user->second;
    }
}

void UserCollection::add(User user, bool isAtomic) {
    if (isAtomic) std::lock_guard _(m);  // cpp17

    userByName[user.getUserName()] = user;
}
