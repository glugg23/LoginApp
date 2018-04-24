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
        std::cerr << "ERROR: Encryption library could not be initialized" << std::endl;
        return 1;
    }

    User user;

    //Test to see how document creation works
    /*bsoncxx::builder::basic::document basic_builder{};
    basic_builder.append(kvp("user", "test"));
    basic_builder.append(kvp("password", "123"));
    bsoncxx::document::value document = basic_builder.extract();
    bsoncxx::document::view view = document.view();
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(view);*/

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

        //Set values of user
        user(username, password);

        //Clear data now that user object has stored it
        username.clear();
        password.clear();

        //Find if document matching username exists
        core::optional<bsoncxx::document::value> maybe_doc
            = collection.find_one(make_document(kvp("user", user.getUsername())));

        if(maybe_doc) {
            //View document
            bsoncxx::document::view view = maybe_doc->view();
            //Get element in password row
            bsoncxx::document::element element = view["password"];

            //Check whether the hash matches the given password
            if(crypto_pwhash_str_verify(element.get_utf8().value.to_string().c_str(),
                 user.getPassword().c_str(), user.getPassword().length()) == 0) {
                user.toggleLoggedIn();
                std::cout << "You have successfully logged in" << std::endl;

            } else {
                std::cout << "Wrong password" << std::endl;
                //Maybe add option here to change password?
            }

        } else {
            std::cout << "Wrong username" << std::endl;
            //Maybe add option here to create new user?
        }

    } while(!user.isLoggedIn());

    return 0;
}