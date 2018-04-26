#include <iostream>

#include <sodium.h>

#include <bsoncxx/exception/exception.hpp>

#include "menu.h"

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

    if(password1 != password2 || password1 != user.getPassword() || password2 != user.getPassword()) {
        std::cout << "Your passwords didn't match.\n"
                  << "Aborting delete attempt." << std::endl;
        return "";
    }

    collection.delete_one(make_document(kvp("username", user.getUsername())));
    std::cout << "User deleted, goodbye!" << std::endl;

    return "exit";
}

void showInfo(User &user, mongocxx::collection &collection) {
    auto document = collection.find_one(make_document(kvp("username", user.getUsername())));

    bsoncxx::document::element element = document->view()["loginCount"];
    //TODO Fix this printing "You have logged in 1 times"
    std::cout << "You have logged in " << element.get_int32() << " times." << std::endl;
}

void menuChoice(std::string &choice, User &user, mongocxx::collection &collection) {
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

    if(choice == "deleteUser") {
        //Bit of a hack to return a string so the program will automatically close
        choice = deleteUser(user, collection);
        return;
    }

    if(choice == "info") {
        showInfo(user, collection);
        return;
    }

    //If not a valid command
    std::cout << choice << ": command not found." << std::endl;
}

void runMenu(User &user, mongocxx::collection &collection) {
    std::cout << "\nType 'help' to see a list of commands you can do.\n"
              << "Type 'exit' to log out.\n"  << std::endl;

    std::string choice;

    auto document = collection.find_one(make_document(kvp("username", user.getUsername())));

    try {
        bsoncxx::document::element element = document->view()["resetPassword"];

        //element can throw an exception if field resetPassword does not exist
        if (element.get_bool().value) {
            std::cout << "You recently reset your password.\n"
                      << "Please change your password.\n" << std::endl;
            changePassword(user, collection);
        }

    } catch(bsoncxx::exception &e) {
        //If this exception is thrown update document to include this field
        collection.update_one(
            make_document(kvp("username", user.getUsername())),
            make_document(kvp("$set", make_document(kvp("resetPassword", false))))
        );
    }

    try {
        bsoncxx::document::element element = document->view()["loginCount"];

        int count = element.get_int32();

        collection.update_one(
            make_document(kvp("username", user.getUsername())),
            make_document(kvp("$set", make_document(kvp("loginCount", ++count))))
        );

    } catch(bsoncxx::exception &e) {
        collection.update_one(
            make_document(kvp("username", user.getUsername())),
            make_document(kvp("$set", make_document(kvp("loginCount", 1))))
        );
    }

    do {
        std::cout << user.getUsername() << ": ";
        std::cin >> choice;

        menuChoice(choice, user, collection);

    } while(choice != "exit");
}