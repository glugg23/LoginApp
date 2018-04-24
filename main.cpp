#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>

#include <sodium.h>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "user.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;

mongocxx::instance instance{};
mongocxx::client client{mongocxx::uri{"mongodb://ds251727.mlab.com:51727/"}};
mongocxx::database db = client["users_test"];
mongocxx::collection coll = db["users"];

int main() {
    if (sodium_init() == -1) {
        std::cerr << "ERROR: Encryption library could not be initialized" << std::endl;
        return 1;
    }

    User user;
    std::fstream inputFile;
    inputFile.open("users.txt", std::ios::in | std::ios::out);

    //Test to see how document creation works
    bsoncxx::builder::basic::document basic_builder{};
    basic_builder.append(kvp("user", "test"));
    basic_builder.append(kvp("password", "123"));
    bsoncxx::document::value document = basic_builder.extract();
    bsoncxx::document::view view = document.view();

    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(view);

    do {
        std::string username, password;
        std::cout << "Enter your username: ";
        std::cin >> username;
        std::cout << "Enter your password: ";
        std::cin >> password;

        //Commented out way of how to hash password, to use at later date
        /*char hashed_password[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str
                (hashed_password, password.c_str(), password.length(),
                 crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
            std::cerr << "ERROR: Out of memory for hash" << std::endl;
            return 1;
        }

        //Print hash for debugging reasons
        std::cout << hashed_password << std::endl;*/

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
            if(user.verifyUser(User(fileUsername, filePassword))) {
                user.toggleLoggedIn();
                std::cout << "You have successfully logged in" << std::endl;
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