#include <utils/banner.hpp>
#include <core/core.hpp>
#include <core/db.hpp>
#include <filesystem>
#include <iostream>
#include <print>
#include <sodium.h>

#ifndef BANKAPP_DB_PATH
#define BANKAPP_DB_PATH "data/bank.db"
#endif

int main()
{
    BannerPrinter printer;
    printer.print_styled_banner("Cyrixon Bank!");

    const std::filesystem::path dbPath{BANKAPP_DB_PATH};
    std::filesystem::create_directories(dbPath.parent_path());

    sqlite3 *db = nullptr;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        std::println(std::cerr, "Cannot open DB: {}", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    initDatabase(db);

    if (sodium_init() < 0)
    {
        std::println(std::cerr, "libsodium init failed");
        sqlite3_close(db);
        return 1;
    }
    Core app(db);
    app.run();

    sqlite3_close(db);
    return 0;
}
