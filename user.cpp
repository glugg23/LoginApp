#include "user.h"

#include <sodium.h>

User::User(const std::string &username, char *password) : username(username), password(password) {}

const std::string &User::getUsername() const {
    return username;
}

void User::setUsername(const std::string &username) {
    User::username = username;
}

char *User::getPassword() const {
    return password;
}

void User::setPassword(char *password) {
    User::password = password;
}

bool User::isLoggedIn() const {
    return loggedIn;
}

void User::toggleLoggedIn() {
    User::loggedIn = !User::loggedIn;
}

bool User::verifyUser(const User &userFromFile) {
    if(username == userFromFile.getUsername()) {
        if((crypto_pwhash_str_verify(password, userFromFile.getPassword(), strlen(userFromFile.getPassword()))) == 0) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}
