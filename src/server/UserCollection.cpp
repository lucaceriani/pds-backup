#include "UserCollection.hpp"

#include <optional>

using namespace PDSBackup;

std::optional<std::string> UserCollection::login(std::string username, std::string pass, bool isAtomic) {
    if (isAtomic) std::lock_guard(m);  // cpp17

    auto user = getUserByName(username);
    if (user.has_value()) {
        // se ho trovasto l'utente provo a fare il login
        user.value().login()
    }
}

std::optional<User> UserCollection::getUserByName(std::string uname, bool isAtomic) {
    if (isAtomic) std::lock_guard(m);  // cpp17

    auto user = userByName.find(uname);
    if (user == userByName.end()) {
        return std::nullopt;
    } else {
        return user->second;
    }
}
std::optional<std::string> UserCollection::getSidByName(std::string uname, bool isAtomic) {
    if (isAtomic) std::lock_guard(m);  // cpp17

    auto sid = sidByName.find(uname);
    if (sid == sidByName.end()) {
        return std::nullopt;
    } else {
        return sid->second;
    }
}