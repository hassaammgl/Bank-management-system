#include <core/db.hpp>
#include <models/transaction.hpp>

bool initDatabase(sqlite3 *db)
{
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    return CustomerTable::create(db) && AccountTable::create(db) && TransactionTable::create(db);
}
