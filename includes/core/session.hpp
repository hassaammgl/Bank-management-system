#pragma once

#include <models/customer.hpp>
#include <models/account.hpp>
#include <chrono>
#include <string>
#include <string_view>

class Session
{
private:
    Customer customer{};
    Account account{};
    bool active{false};
    std::chrono::system_clock::time_point login_time{};

public:
    Session() = default;

    Session(Customer customer_, Account account_)
        : customer(std::move(customer_)),
          account(std::move(account_)),
          active(true),
          login_time(std::chrono::system_clock::now())
    {
    }

    // Status checks
    [[nodiscard]] bool is_active() const noexcept { return active; }

    // Customer accessors
    [[nodiscard]] const Customer &get_customer() const noexcept { return customer; }
    [[nodiscard]] Customer &get_customer() noexcept { return customer; }
    void set_customer(Customer c) { customer = std::move(c); }

    [[nodiscard]] std::string_view get_customer_name() const noexcept { return customer.name; }
    [[nodiscard]] std::string_view get_customer_email() const noexcept { return customer.email; }
    [[nodiscard]] std::string_view get_customer_id() const noexcept { return customer._id; }

    // Account accessors
    [[nodiscard]] const Account &get_account() const noexcept { return account; }
    [[nodiscard]] Account &get_account() noexcept { return account; }
    void set_account(Account a) { account = std::move(a); }

    [[nodiscard]] std::string_view get_account_number() const noexcept { return account.account_number; }
    [[nodiscard]] int64_t get_balance() const noexcept { return account.balance; }
    void update_balance(int64_t new_balance) noexcept { account.balance = new_balance; }

    // Lifecycle
    [[nodiscard]] std::chrono::system_clock::time_point get_login_time() const noexcept { return login_time; }
    void end() noexcept { active = false; }
};
