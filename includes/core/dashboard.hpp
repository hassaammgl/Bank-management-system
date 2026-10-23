#pragma once

#include <string_view>
#include <print>
#include <sqlite3.h>
#include <core/session.hpp>
#include <utils/banner.hpp>

class Dashboard
{
public:
    enum class Action
    {
        Logout,
        Exit
    };

    explicit Dashboard(sqlite3 *db);
    Action run(Session &session);

private:
    sqlite3 *db;
    BannerPrinter printer;

    void showMenu();
    void check_balance(const Session &session);
    void deposit_money(Session &session);
    void not_implemented(std::string_view what, const Session &session);
};