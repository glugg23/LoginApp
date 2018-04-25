#pragma once

#include <sodium.h>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "user.h"

void makeNewUser(User &user, mongocxx::collection &collection);

void changePassword(User &user, mongocxx::collection &collection);