#pragma once

#include <mongocxx/client.hpp>

#include "user.h"

void menuChoice(const std::string &choice, User &user, mongocxx::collection &collection);

void runMenu(User &user, mongocxx::collection &collection);