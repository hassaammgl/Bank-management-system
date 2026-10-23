#include <models/transaction.hpp>
#include <sqlite3.h>
#include <iostream>

bool TransactionTable::create(sqlite3 *db)
{
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            _id        TEXT PRIMARY KEY,
            account_id TEXT NOT NULL,
            type       INTEGER NOT NULL DEFAULT 0,
            amount     INTEGER NOT NULL DEFAULT 0,
            detail     TEXT NOT NULL DEFAULT '',
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (account_id) REFERENCES accounts(_id)
        );
        CREATE INDEX IF NOT EXISTS idx_transactions_account_id
            ON transactions (account_id);
    )";

    char *err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cerr << "transactions table error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool TransactionTable::insert(sqlite3 *db, const Transaction &t)
{
    const char *sql =
        "INSERT INTO transactions (_id, account_id, type, amount, detail) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, t._id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, t.account_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, static_cast<int>(t.type));
    sqlite3_bind_int64(stmt, 4, t.amount);
    sqlite3_bind_text(stmt, 5, t.detail.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        std::cerr << "insert error: " << sqlite3_errmsg(db) << "\n";

    sqlite3_finalize(stmt);
    return ok;
}

std::vector<Transaction> TransactionTable::find_by_account(sqlite3 *db, const std::string &account_id)
{
    const char *sql =
        "SELECT _id, account_id, type, amount, detail, created_at "
        "FROM transactions WHERE account_id = ? ORDER BY rowid;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {};

    sqlite3_bind_text(stmt, 1, account_id.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<Transaction> found;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto column = [stmt](int index) -> std::string
        {
            const unsigned char *text = sqlite3_column_text(stmt, index);
            return text ? reinterpret_cast<const char *>(text) : std::string{};
        };

        Transaction t;
        t._id = column(0);
        t.account_id = column(1);
        t.type = static_cast<TransactionType>(sqlite3_column_int(stmt, 2));
        t.amount = sqlite3_column_int64(stmt, 3);
        t.detail = column(4);
        t.created_at = column(5);
        found.push_back(std::move(t));
    }

    sqlite3_finalize(stmt);
    return found;
}
