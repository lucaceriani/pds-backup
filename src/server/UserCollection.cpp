#include "UserCollection.hpp"

#include <optional>

using namespace PDSBackup;

std::optional<std::string> UserCollection::login(std::string username, std::string pass, bool isAtomic) {
    if (isAtomic) std::unique_lock _(sm);

    auto optUser = getUserByName(username);
    if (optUser.has_value()) {
        // se ho trovasto l'utente provo a fare il login
        auto optSid = optUser.value().login(username, pass);
        if (optSid.has_value()) {
            // statisticamente e' molto improbabile che si generino due sid uguali
            // salvo nella mappa stringaSid -> objSid
            sessionIdMap[optSid.value().sessionId] = optSid.value();

            //ritorno la stringa
            return optSid.value().sessionId;
        }
    }

    return std::nullopt;
}

std::optional<User> UserCollection::getUserByName(std::string uname, bool isAtomic) {
    if (isAtomic) std::shared_lock _(sm);

    auto user = userByName.find(uname);
    if (user == userByName.end()) {
        return std::nullopt;
    } else {
        return user->second;
    }
}

void UserCollection::add(User user, bool isAtomic) {
    if (isAtomic) std::unique_lock _(sm);

    userByName[user.getUserName()] = user;
}

std::optional<SessionId> UserCollection::getSessionId(std::string sid, bool isAtomic) {
    if (isAtomic) std::shared_lock _(sm);

    auto found = sessionIdMap.find(sid);

    if (found == sessionIdMap.end())
        return std::nullopt;

    // se l'ho trovato ed e' scaduto
    if (found->second.isExpired())
        return std::nullopt;

    return found->second;
}
