#include <models/customer.hpp>
#include <iostream>
#include <utility>

bool CustomerTable::create(sqlite3 *db)
{
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS customers (
            _id       TEXT PRIMARY KEY,
            name      TEXT NOT NULL,
            email     TEXT NOT NULL UNIQUE,
            password  TEXT NOT NULL,
            phone     TEXT,
            address   TEXT
        );
    )";

    char *err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cerr << "customers table error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool CustomerTable::insert(sqlite3 *db, const Customer &c)
{
    const char *sql =
        "INSERT INTO customers (_id, name, email, password, phone, address) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, c._id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, c.password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, c.phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, c.address.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        std::cerr << "insert error: " << sqlite3_errmsg(db) << "\n";

    sqlite3_finalize(stmt);
    return ok;
}

std::optional<Customer> CustomerTable::find_by_email(sqlite3 *db, const std::string &email)
{
    const char *sql =
        "SELECT _id, name, email, password, phone, address "
        "FROM customers WHERE email = ? LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Customer> found;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto column = [stmt](int index) -> std::string
        {
            const unsigned char *text = sqlite3_column_text(stmt, index);
            return text ? reinterpret_cast<const char *>(text) : std::string{};
        };

        Customer c;
        c._id = column(0);
        c.name = column(1);
        c.email = column(2);
        c.password = column(3);
        c.phone = column(4);
        c.address = column(5);
        found = std::move(c);
    }

    sqlite3_finalize(stmt);
    return found;
}
