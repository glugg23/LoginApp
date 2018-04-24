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

void User::operator()(const std::string &username, const std::string &password) {
    this->username = username;
    this->password = password;
}