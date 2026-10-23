#pragma once

#include <string>
#include <optional>
#include <sqlite3.h>
#include <constants/index.hpp>

struct Customer
{
    std::string _id{};
    std::string name{};
    std::string email{};
    std::string password{};
    std::string phone{};
    std::string address{};
};

namespace CustomerTable
{
    inline constexpr const char *NAME = "customers";
    bool create(sqlite3 *db);
    bool insert(sqlite3 *db, const Customer &c);
    std::optional<Customer> find_by_email(sqlite3 *db, const std::string &email);
}
