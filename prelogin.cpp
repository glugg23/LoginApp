#include "prelogin.h"

#include <iostream>
#include <random>

#include <sodium.h>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void makeNewUser(User &user, mongocxx::collection &collection) {
    char hashedPassword[crypto_pwhash_STRBYTES];

    if(crypto_pwhash_str
            (hashedPassword, user.getPassword().c_str(), user.getPassword().length(),
             crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) != 0) {
        std::cerr << "ERROR: Out of memory for hash." << std::endl;
        return;
    }

    bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
        collection.insert_one(make_document(kvp("user", user.getUsername()), kvp("password", hashedPassword)));

    if(result) {
        std::cout << "Your account was created!" << std::endl;
        user.toggleLoggedIn();
        std::cout << "You have successfully logged in!" << std::endl;

    } else {
        std::cerr << "ERROR: Something unexpected happened." << std::endl;
    }
}

std::string randomString() {
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, 12);
}

void changePasswordPreLogin(User &user, mongocxx::collection &collection) {
    user.setPassword(randomString());

    char hashedPassword[crypto_pwhash_STRBYTES];

    if(crypto_pwhash_str
           (hashedPassword, user.getPassword().c_str(), user.getPassword().length(),
            crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
        std::cerr << "ERROR: Out of memory for hash." << std::endl;
        return;
    }

    collection.update_one(
        make_document(kvp("user", user.getUsername())),
        make_document(kvp("$set", make_document(kvp("password", hashedPassword), kvp("resetPassword", true))))
    );

    std::cout << "Your password was reset.\n"
              << "Your new password is " << user.getPassword() << '\n'
              << "Once you have logged in, please change it to something else." << std::endl;
}