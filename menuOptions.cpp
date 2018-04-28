#include <iostream>

#include <sodium.h>

#include <bsoncxx/exception/exception.hpp>

#include "menuOptions.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void printHelp() {
    std::cout << "help: displays this help menu." << std::endl;
    std::cout << "changepw: allows you to change your password." << std::endl;
    std::cout << "deleteUser: deletes your account." << std::endl;
    std::cout << "info: shows you info about your account." << std::endl;
    std::cout << "exit: logs you out." << std::endl;
}

void changePassword(User &user, mongocxx::collection &collection) {
    std::string password1;
    std::string password2;

    //Enter new password twice
    do {
        std::cout << "Enter your new password: ";
        std::cin >> password1;

        std::cout << "Enter your new password a second time: ";
        std::cin >> password2;

        if(password1 != password2) {
            std::cout << "Your passwords didn't match, please try again." << std::endl;
        }

    } while(password1 != password2);

    user.setPassword(password1);

    password1.clear();
    password2.clear();

    //Hash password
    char hashedPassword[crypto_pwhash_STRBYTES];

    if(crypto_pwhash_str
           (hashedPassword, user.getPassword().c_str(), user.getPassword().length(),
            crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
        std::cerr << "ERROR: Out of memory for hash." << std::endl;
        return;
    }

    //Update db
    collection.update_one(
        make_document(kvp("username", user.getUsername())),
        make_document(kvp("$set", make_document(kvp("password", hashedPassword), kvp("resetPassword", false))))
    );

    std::cout << "Your password has been changed." << std::endl;
}

std::string deleteUser(User &user, mongocxx::collection &collection) {
    std::string password1;
    std::string password2;

    std::cout << "Enter your password to delete your account: ";
    std::cin >> password1;

    std::cout << "Enter your password a second time: ";
    std::cin >> password2;

    //Make sure all passwords match and quit if any don't
    if(password1 != password2 || password1 != user.getPassword() || password2 != user.getPassword()) {
        std::cout << "Your passwords didn't match.\n"
                  << "Aborting delete attempt." << std::endl;
        return "";
    }

    //Remove from db
    collection.delete_one(make_document(kvp("username", user.getUsername())));
    std::cout << "User deleted, goodbye!" << std::endl;

    //Return exit so that app closes
    return "exit";
}

void showInfo(User &user, mongocxx::collection &collection) {
    auto document = collection.find_one(make_document(kvp("username", user.getUsername())));

    bsoncxx::document::element element = document->view()["loginCount"];

    core::optional<bsoncxx::document::element> element1 = document->view()["loginCount"];

    if(element.get_int32() == 1) {
        std::cout << "You have logged in 1 time." << std::endl;

    } else {
        std::cout << "You have logged in " << element.get_int32() << " times." << std::endl;
    }

    element = document->view()["lastLogin"];

    std::time_t time = std::chrono::duration_cast<std::chrono::seconds>(element.get_date().value).count();
    std::cout << "You last logged in on " << std::ctime(&time);

    element = document->view()["_id"];

    time = element.get_oid().value.get_time_t();
    std::cout << "You created this account on " << std::ctime(&time);

    std::cout << '\n';
}