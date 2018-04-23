#pragma once

#include <iostream>

class User {
private:
    std::string username;
    std::string password;
    char* hash;
    bool loggedIn = false;

public:
    User() = default;

    User(const std::string &username, const std::string &password);

    const std::string &getUsername() const;

    void setUsername(const std::string &username);

    const std::string &getPassword() const;

    void setPassword(const std::string &password);

    char *getHash() const;

    void setHash(char *hash);

    bool isLoggedIn() const;

    void toggleLoggedIn();

    bool operator==(const User &rhs) const;

    bool operator!=(const User &rhs) const;
};