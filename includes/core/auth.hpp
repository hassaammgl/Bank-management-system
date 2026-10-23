#pragma once

#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <models/customer.hpp>
#include <utils/banner.hpp>
#include <print>
#include <sqlite3.h>

enum class ValidationError
{
    EmptyFields,
    PasswordMismatch,
    InvalidEmail,
    InvalidPhone,
    InvalidCNIC
};

inline std::string_view errorToString(ValidationError err)
{
    switch (err)
    {
    case ValidationError::EmptyFields:
        return "All fields are required!";
    case ValidationError::PasswordMismatch:
        return "Passwords do not match!";
    case ValidationError::InvalidEmail:
        return "Invalid Email format! (e.g., user@example.com)";
    case ValidationError::InvalidPhone:
        return "Invalid Phone format! (e.g., 03001234567 or +923001234567)";
    case ValidationError::InvalidCNIC:
        return "Invalid CNIC format! (e.g., 35201-1234567-1)";
    }
    return "Unknown validation error!";
}

struct Login
{
    std::string email{};
    std::string password{};
};
struct Register
{
    std::string name{};
    std::string email{};
    std::string phone{};
    std::string address{};
    std::string cnic{};
    std::string password{};
    std::string cpassword{};
};

class Auth
{
private:
    sqlite3 *db;
    BannerPrinter printer;
    Login lgn;
    Register reg;
    unsigned short showMenu();
    std::expected<Register, ValidationError> collectRegistrationData();

public:
    explicit Auth(sqlite3 *db);
    // Auth menu loop: login/register success par customer, Exit (0) par nullopt.
    std::optional<Customer> run();
    std::optional<Customer> login();
    std::optional<Customer> _register();
};