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

#include "PRIVATE.h"
#include "user.h"
#include "database.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int main() {
    mongocxx::instance instance{};
    mongocxx::client client{mongocxx::uri{DATABASE_LOGIN_ADMIN}};
    mongocxx::database db = client["users_test"];
    mongocxx::collection collection = db["users"];

    if (sodium_init() == -1) {
        std::cerr << "ERROR: Encryption library could not be initialized." << std::endl;
        return 1;
    }

    User user;
    int count = 0;

    do {
        std::string username, password;
        std::cout << "Enter your username: ";
        std::cin >> username;
        std::cout << "Enter your password: ";
        std::cin >> password;

        //Set values of user
        user(username, password);

        //Clear data now that user object has stored it
        username.clear();
        password.clear();

        //Find if document matching username exists
        core::optional<bsoncxx::document::value> maybeDoc
            = collection.find_one(make_document(kvp("user", user.getUsername())));

        if(maybeDoc) {
            //Get element in password row
            bsoncxx::document::element element = maybeDoc->view()["password"];

            //Check whether the hash matches the given password
            if(crypto_pwhash_str_verify(element.get_utf8().value.to_string().c_str(),
                 user.getPassword().c_str(), user.getPassword().length()) == 0) {
                user.toggleLoggedIn();
                std::cout << "You have successfully logged in!" << std::endl;

            } else {
                std::cout << "Incorrect password." << std::endl;
                ++count;

                if(count >= 3) {
                    char input;
                    do {
                        std::cout << "Would you like to change your password? (y/n): ";
                        std::cin >> input;

                        if(input == 'y' || input == 'Y') {
                            changePassword(user, collection);
                            count = 0;
                            break;
                        }

                    } while(!(input == 'n' || input == 'N'));
                }
            }

        } else {
            std::cout << "This user does not exist." << std::endl;
            char input;
            do {
                std::cout << "Would you like to create a new user? (y/n): ";
                std::cin >> input;

                if(input == 'y' || input == 'Y') {
                    makeNewUser(user, collection);
                    break;
                }

            } while(!(input == 'n' || input == 'N'));
        }

    } while(!user.isLoggedIn());

    return 0;
}