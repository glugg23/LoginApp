#include "database.h"

#include <iostream>

using bsoncxx::builder::basic::kvp;

void makeNewUser(User &user, mongocxx::collection &collection) {
    std::string password1;
    std::string password2;

    do {
        std::cout << "Enter your password: ";
        std::cin >> password1;

        std::cout << "Enter your password a second time: ";
        std::cin >> password2;

        if(password1 != password2) {
            std::cout << "Your passwords didn't match, please try again" << std::endl;
        }

    } while(password1 != password2);

    user.setPassword(password1);

    password1.clear();
    password2.clear();

    char hashed_password[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str
            (hashed_password, user.getPassword().c_str(), user.getPassword().length(),
             crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
        std::cerr << "ERROR: Out of memory for hash" << std::endl;
        return;
    }

    bsoncxx::builder::basic::document basic_builder{};
    basic_builder.append(kvp("user", user.getUsername()));
    basic_builder.append(kvp("password", hashed_password));
    bsoncxx::document::value document = basic_builder.extract();
    bsoncxx::document::view view = document.view();
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(view);

    if(result) {
        std::cout << "Your account was created!" << std::endl;

    } else {
        std::cerr << "ERROR: Something unexpected happened" << std::endl;
    }
}