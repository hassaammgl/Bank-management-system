#pragma once

#include <optional>
#include <core/auth.hpp>
#include <core/dashboard.hpp>
#include <core/session.hpp>
#include <sqlite3.h>

class Core
{
private:
    sqlite3 *db;
    Auth auth;
    Dashboard _dashboard;
    std::optional<Session> session{};   // login hone par set, logout par clear

public:
    explicit Core(sqlite3 *db);
    // Pura app: jab tak running hai — session nahi hai to Auth, hai to Dashboard.
    void run();
};
