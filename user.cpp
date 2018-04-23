#include "user.h"

User::User(const std::string &username, const std::string &password) : username(username), password(password) {}

const std::string &User::getUsername() const {
    return username;
}

void User::setUsername(const std::string &username) {
    User::username = username;
}

const std::string &User::getPassword() const {
    return password;
}

void User::setPassword(const std::string &password) {
    User::password = password;
}

char* User::getHash() const {
    return hash;
}

void User::setHash(char *hash) {
    User::hash = hash;
}

bool User::isLoggedIn() const {
    return loggedIn;
}

void User::toggleLoggedIn() {
    User::loggedIn = !User::loggedIn;
}


bool User::operator==(const User &rhs) const {
    return username == rhs.username &&
           password == rhs.password;
}

bool User::operator!=(const User &rhs) const {
    return !(rhs == *this);
}
