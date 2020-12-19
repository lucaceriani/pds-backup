#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <optional>
#include <string>

namespace PDSBackup {

class SessionId {
   public:
    SessionId();
    std::string sessionId;
    std::string expiresAt;
    std::string owner;
    bool isExpired();
};

class User {
   public:
    User() {}
    User(std::string uname, std::string salt, std::string hpass);

    // getters
    std::string getSessionId();
    std::string getStringExpirationDate();
    std::string getUserName();

    // check
    bool isExpired();
    bool checkSessionId(std::string sid);

    // login m
    std::optional<SessionId> login(std::string user, std::string pass);

   private:
    boost::posix_time::ptime expiresAt;
    std::string userName;
    std::string userHPassword;
    std::string userSalt;

    SessionId generateSessionId(std::string user);
};
}  // namespace PDSBackup