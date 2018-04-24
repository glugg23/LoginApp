#pragma once

#include <iostream>

class User {
private:
    std::string username;
    char *password = nullptr;
    bool loggedIn = false;

public:
    User() = default;

    User(const std::string &username, char *password);

    const std::string &getUsername() const;

    void setUsername(const std::string &username);

    char* getPassword() const;

    void setPassword(char *password);

    bool isLoggedIn() const;

    void toggleLoggedIn();

    bool verifyUser(const User &userFromFile);
};