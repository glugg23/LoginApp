#include <iostream>
#include <fstream>
#include <sstream>

#include "user.h"

int main() {
    std::string username, password;
    User user;

    std::fstream inputFile;
    inputFile.open("users.txt", std::ios::in | std::ios::out);

    do {
        std::cout << "Enter your username: ";
        std::cin >> username;
        std::cout << "Enter your password: ";
        std::cin >> password;

        user.setUsername(username);
        user.setPassword(password);

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