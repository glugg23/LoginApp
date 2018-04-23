#include <iostream>
#include <fstream>
#include <sstream>

#include <sodium.h>

#include "user.h"

int main() {
    if (sodium_init() == -1) {
        std::cerr << "ERROR: Encryption library could not be initialized" << std::endl;
        return 1;
    }

    std::string username, password;
    User user;

    std::fstream inputFile;
    inputFile.open("users.txt", std::ios::in | std::ios::out);

    do {
        std::cout << "Enter your username: ";
        std::cin >> username;
        std::cout << "Enter your password: ";
        std::cin >> password;

        char hashed_password[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str
                (hashed_password, password.c_str(), password.length(),
                 crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
            std::cerr << "ERROR: Out of memory for hash" << std::endl;
            return 1;
        }

        user.setUsername(username);
        user.setPassword(password);
        user.setHash(hashed_password);

        //Read file line by line
        for(std::string line; std::getline(inputFile, line);) {
            std::istringstream iss(line);
            std::string fileUsername, filePassword;

            //Splits string and breaks in case it doesn't work
            if(!(iss >> fileUsername >> filePassword)) {
                break;
            }

            //If the username and password match, login user and end loop
            if(user.getUsername() == fileUsername && user.getPassword() == filePassword) {
                user.toggleLoggedIn();
                break;
            }
        }

        //If the user failed to login
        if(!user.isLoggedIn()) {
            std::cout << "Wrong username or password." << std::endl;

            //Reset file and seek to beginning
            inputFile.clear();
            inputFile.seekg(0, std::ios::beg);
        }

    } while(!user.isLoggedIn());

    inputFile.close();

    return 0;
}