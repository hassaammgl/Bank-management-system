#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <sqlite3.h>

enum class TransactionType
{
    Deposit = 0,
    Withdraw,
    Transfer
};

struct Transaction
{
    using Money = std::int64_t;
    std::string _id{};
    std::string account_id{};   // FK: accounts._id
    TransactionType type{TransactionType::Deposit};
    Money amount{};             // positive; direction type se aati hai
    std::string detail{};       // e.g. "Deposit", "Withdraw", "Transfer to acc#..."
    std::string created_at{};   // UTC (SQLite CURRENT_TIMESTAMP)
};

namespace TransactionTable
{
    inline constexpr const char *NAME = "transactions";
    bool create(sqlite3 *db);
    bool insert(sqlite3 *db, const Transaction &t);
    std::vector<Transaction> find_by_account(sqlite3 *db, const std::string &account_id);
}
