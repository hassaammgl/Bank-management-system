#include <models/account.hpp>
#include <sqlite3.h>
#include <iostream>

bool AccountTable::create(sqlite3 *db)
{
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS accounts (
            _id            TEXT PRIMARY KEY,
            account_number TEXT NOT NULL UNIQUE,
            customer_id    TEXT NOT NULL,
            account_type   INTEGER NOT NULL DEFAULT 0,
            balance        INTEGER NOT NULL DEFAULT 0,
            status         INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY (customer_id) REFERENCES customers(_id)
        );
    )";

    char *err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cerr << "accounts table error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool AccountTable::insert(sqlite3 *db, const Account &a)
{
    const char *sql =
        "INSERT INTO accounts (_id, account_number, customer_id, account_type, balance, status) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, a._id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, a.account_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, a.linkedCustomer.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, static_cast<int>(a.account_type));
    sqlite3_bind_int64(stmt, 5, a.balance);
    sqlite3_bind_int(stmt, 6, static_cast<int>(a.account_status));

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        std::cerr << "insert error: " << sqlite3_errmsg(db) << "\n";

    sqlite3_finalize(stmt);
    return ok;
}

std::optional<Account> AccountTable::find_by_customer(sqlite3 *db, const std::string &customer_id)
{
    const char *sql =
        "SELECT _id, account_number, account_type, balance, status "
        "FROM accounts WHERE customer_id = ? ORDER BY rowid LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_text(stmt, 1, customer_id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Account> found;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto column = [stmt](int index) -> std::string
        {
            const unsigned char *text = sqlite3_column_text(stmt, index);
            return text ? reinterpret_cast<const char *>(text) : std::string{};
        };

        Account a;
        a._id = column(0);
        a.account_number = column(1);
        a.account_type = static_cast<AccountType>(sqlite3_column_int(stmt, 2));
        a.balance = sqlite3_column_int64(stmt, 3);
        a.account_status = static_cast<AccountStatus>(sqlite3_column_int(stmt, 4));
        a.linkedCustomer = customer_id;
        found = std::move(a);
    }

    sqlite3_finalize(stmt);
    return found;
}

std::optional<Account::Money> AccountTable::get_balance(sqlite3 *db, const std::string &customer_id)
{
    const char *sql =
        "SELECT  balance FROM accounts "
        "WHERE customer_id = ? ORDER BY rowid LIMIT 1;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_text(stmt, 1, customer_id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Account::Money> balance;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        balance = sqlite3_column_int64(stmt, 0);

    sqlite3_finalize(stmt);
    return balance;
}

std::optional<Account> AccountTable::update_balance(sqlite3 *db,
                                                    const std::string &customer_id,
                                                    Account::Money delta)
{
    const char *sql =
        "UPDATE accounts SET balance = balance + ?1 "
        "WHERE customer_id = ?2 "
        "  AND account_status = 0 "
        "  AND balance + ?1 >= 0;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_int64(stmt, 1, delta);
    sqlite3_bind_text(stmt, 2, customer_id.c_str(), -1, SQLITE_TRANSIENT);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE || sqlite3_changes(db) == 0)
        return std::nullopt;

    return find_by_customer(db, customer_id);
}