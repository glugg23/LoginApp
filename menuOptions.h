#pragma once

#include <mongocxx/client.hpp>

#include "user.h"

void printHelp();

void changePassword(User &user, mongocxx::collection &collection);

std::string deleteUser(User &user, mongocxx::collection &collection);

void showInfo(User &user, mongocxx::collection &collection);