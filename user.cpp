#include "user.h"

#include <sodium.h>

User::User(const std::string &username, const std::string &password) : username(username), password(password) {}

const std::string &User::getUsername() const {
    return username;
}

void User::setUsername(const std::string &username) {
    User::username = username;
}

std::string User::getPassword() const {
    return password;
}

void User::setPassword(const std::string &password) {
    User::password = password;
}

bool User::isLoggedIn() const {
    return loggedIn;
}

void User::toggleLoggedIn() {
    User::loggedIn = !User::loggedIn;
}

bool User::verifyUser(const User &userFromFile) {
    //Only run slow function if usernames match
    if(username == userFromFile.getUsername()) {
        return crypto_pwhash_str_verify(userFromFile.getPassword().c_str(),
               password.c_str(), password.length()) == 0;

    } else {
        return false;
    }
}