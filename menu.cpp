#include <iostream>

#include <sodium.h>

#include "menu.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void printHelp() {
    std::cout << "help: displays this help menu." << std::endl;
    std::cout << "changepw: allows you to change your password." << std::endl;
    std::cout << "exit: logs you out." << std::endl;
}

void changePassword(User &user, mongocxx::collection &collection) {
    std::string password1;
    std::string password2;

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

    char hashedPassword[crypto_pwhash_STRBYTES];

    if(crypto_pwhash_str
           (hashedPassword, user.getPassword().c_str(), user.getPassword().length(),
            crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
        std::cerr << "ERROR: Out of memory for hash." << std::endl;
        return;
    }

    collection.update_one(
        make_document(kvp("user", user.getUsername())),
        make_document(kvp("$set", make_document(kvp("password", hashedPassword), kvp("resetPassword", false))))
    );
}

void menuChoice(const std::string &choice, User &user, mongocxx::collection &collection) {
    if(choice == "help") {
        printHelp();
        return;
    }

    if(choice == "exit") {
        return;
    }

    if(choice == "changepw") {
        changePassword(user, collection);
        return;
    }

    //If not a valid command
    std::cout << choice << ": command not found." << std::endl;
}

void runMenu(User &user, mongocxx::collection &collection) {
    std::cout << "\nType 'help' to see a list of commands you can do.\n"
              << "Type 'exit' to log out.\n"  << std::endl;

    std::string choice;

    core::optional<bsoncxx::document::value> maybeDoc
        = collection.find_one(make_document(kvp("user", user.getUsername())));

    if(maybeDoc) {
        bsoncxx::document::element element = maybeDoc->view()["resetPassword"];

        if(element.get_bool().value) {
            std::cout << "You recently reset your password.\n"
                      << "Please change your password.\n" << std::endl;
            changePassword(user, collection);
        }
    }

    do {
        std::cout << user.getUsername() << ": ";
        std::cin >> choice;

        menuChoice(choice, user, collection);

    } while(choice != "exit");
}