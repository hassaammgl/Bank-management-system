#pragma once

#include <sqlite3.h>
#include <models/customer.hpp>
#include <models/account.hpp>

bool initDatabase(sqlite3 *db);
