#pragma once

#include <mongocxx/client.hpp>

#include "user.h"

void makeNewUser(User &user, mongocxx::collection &collection);

void changePasswordPreLogin(User &user, mongocxx::collection &collection);