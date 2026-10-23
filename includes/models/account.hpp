#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <sqlite3.h>

enum class AccountStatus
{
    Active = 0,
    Blocked
};

enum class AccountType
{
    Current,
    Saving,
    Business
};

struct Account
{
    using Money = std::int64_t;
    std::string _id{};
    std::string account_number{};
    AccountType account_type{AccountType::Current};
    Money balance{};
    AccountStatus account_status{AccountStatus::Active};
    std::string linkedCustomer{};
};

namespace AccountTable
{
    inline constexpr const char *NAME = "accounts";
    bool create(sqlite3 *db);
    bool insert(sqlite3 *db, const Account &a);
    std::optional<Account> find_by_customer(sqlite3 *db, const std::string &customer_id);
    std::optional<Account> update_balance(sqlite3 *db, const std::string &customer_id, Account::Money delta);
    std::optional<Account::Money> get_balance(sqlite3 *db, const std::string &customer_id);

}
