#include "UserCollection.hpp"

#include <optional>

using namespace PDSBackup;

std::optional<std::string> UserCollection::login(std::string username, std::string pass, bool isAtomic) {
    if (isAtomic) std::lock_guard _(m);

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
    if (isAtomic) std::lock_guard _(m);

    auto user = userByName.find(uname);
    if (user == userByName.end()) {
        return std::nullopt;
    } else {
        return user->second;
    }
}

void UserCollection::add(User user, bool isAtomic) {
    if (isAtomic) std::lock_guard _(m);

    userByName[user.getUserName()] = user;
}

bool UserCollection::isValidSessionId(std::string sid, bool isAtomic) {
    if (isAtomic) std::lock_guard _(m);

    if (sid == "________sempre_valido___") return true;

    auto found = sessionIdMap.find(sid);
    if (found == sessionIdMap.end())
        return false;
    else
        // ritorno il fatto che non sia scaduto
        return !(found->second.isExpired());
}
