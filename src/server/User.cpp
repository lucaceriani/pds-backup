#include "User.hpp"

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <iomanip>
#include <sstream>

#include "../shared/Checksum.hpp"
#include "../shared/Protocol.hpp"

using namespace PDSBackup;
using namespace boost::posix_time;

User::User() {
}

User::User(std::string sid, std::string expdate, std::string uname, std::string salt, std::string hpass) {
    sessionId = sid;
    expiresAt = from_iso_string(expdate);
    userName = uname;
    userHPassword = hpass;
    userSalt = salt;
}

std::optional<SessionId> User::login(std::string user, std::string pass) {
    if (Checksum::sha3(pass + userSalt) == userHPassword) {
        return generateSessionId();
    } else {
        return std::nullopt;
    }
}

SessionId User::generateSessionId() {
    SessionId sid;

    // true randomness generator per sicurezza
    boost::random_device rd;
    boost::random::uniform_int_distribution<> dis(0, Protocol::sessionIdCharacters.length() - 1);
    for (unsigned int i = 0; i < Protocol::sessionIdLength; i++) {
        sid.sessionId.push_back(Protocol::sessionIdCharacters[dis(rd)]);
    }

    auto expiresAt = ptime(second_clock::universal_time()) +
                     hours(Protocol::sessionIdValidDays * 24);

    sid.expiresAt = to_iso_string(expiresAt);

    return sid;
}

std::string User::getUserName() {
    return userName;
}

SessionId::SessionId() {
    sessionId.reserve(Protocol::sessionIdLength);
}
