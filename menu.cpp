#include <iostream>

#include <bsoncxx/exception/exception.hpp>

#include "menu.h"
#include "menuOptions.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

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
        //Check to see if password needs to be reset
        bsoncxx::document::element element = document->view()["resetPassword"];

        //element can throw an exception if field resetPassword does not exist
        if(element.get_bool().value) {
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

    //Increase login counter
    bsoncxx::document::element element = document->view()["loginCount"];

    int count = element.get_int32();

    collection.update_one(
        make_document(kvp("username", user.getUsername())),
        make_document(kvp("$set", make_document(kvp("loginCount", ++count))))
    );

    do {
        std::cout << user.getUsername() << ": ";
        std::cin >> choice;

        menuChoice(choice, user, collection);

    } while(choice != "exit");

    //Before quiting add current time to document
    //If deleteUser is called won't do anything bad as it can't find any document to update
    collection.update_one(
        make_document(kvp("username", user.getUsername())),
        make_document(kvp("$set", make_document(kvp("lastLogin", bsoncxx::types::b_date(std::chrono::system_clock::now())))))
    );
}