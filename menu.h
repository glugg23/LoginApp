#pragma once

#include <mongocxx/client.hpp>

#include "user.h"

void runMenu(User &user, mongocxx::collection &collection);