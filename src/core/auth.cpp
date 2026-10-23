#include <core/auth.hpp>
#include <utils/utils.hpp>
#include <utils/sodium.hpp>
#include <models/customer.hpp>
#include <models/account.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

Auth::Auth(sqlite3 *db_) : db(db_) {}

unsigned short Auth::showMenu()
{
    unsigned short choice{};
    printer.print_styled_banner("Auth !");
    std::println("1) Login System.");
    std::println("2) Register System");
    std::println("0) Exit");
    std::cout << "> ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

std::optional<Customer> Auth::run()
{
    while (true)
    {
        switch (unsigned short choice = showMenu(); choice)
        {
        case 1:
            printer.print_styled_banner("Login !");
            if (auto customer = login())
                return customer;
            break;
        case 2:
            printer.print_styled_banner("Register !");
            if (auto customer = _register())
                return customer;
            break;
        case 0:
            printer.print_styled_banner("Good Bye!!");
            return std::nullopt;
        default:
            break;
        }
    }
}

std::optional<Customer> Auth::login()
{
    std::println("Email: ");
    std::cout << "> ";
    std::getline(std::cin, lgn.email);
    std::println("Password: ");
    std::cout << "> ";
    std::getline(std::cin, lgn.password);

    auto customer = CustomerTable::find_by_email(db, lgn.email);
    if (!customer || !SodiumAuth::verify_password(customer->password, lgn.password))
    {
        std::println(std::cerr, "Invalid email or password.");
        return std::nullopt;
    }

    std::println("Welcome back, {}!", customer->name);
    return customer;
}

std::expected<Register, ValidationError> Auth::collectRegistrationData()
{
    std::println("Name: ");
    std::cout << "> ";
    std::getline(std::cin, reg.name);
    std::println("Email: ");
    std::cout << "> ";
    std::getline(std::cin, reg.email);
    std::println("Phone: ");
    std::cout << "> ";
    std::getline(std::cin, reg.phone);
    std::println("CNIC: ");
    std::cout << "> ";
    std::getline(std::cin, reg.cnic);
    std::println("Address: ");
    std::cout << "> ";
    std::getline(std::cin, reg.address);
    std::println("Password: ");
    std::cout << "> ";
    std::getline(std::cin, reg.password);
    std::println("Confirm Password: ");
    std::cout << "> ";
    std::getline(std::cin, reg.cpassword);

    if (reg.name.empty() || reg.password.empty())
    {
        return std::unexpected(ValidationError::EmptyFields);
    }

    if (reg.password != reg.cpassword)
    {
        return std::unexpected(ValidationError::PasswordMismatch);
    }

    if (!isValidEmail(reg.email))
    {
        return std::unexpected(ValidationError::InvalidEmail);
    }

    if (!isValidPhone(reg.phone))
    {
        return std::unexpected(ValidationError::InvalidPhone);
    }

    if (!isValidCNIC(reg.cnic))
    {
        return std::unexpected(ValidationError::InvalidCNIC);
    }

    return reg;
}

std::optional<Customer> Auth::_register()
{
    auto result = collectRegistrationData();
    if (!result)
    {
        std::println(std::cerr, "Registration Failed: {}", errorToString(result.error()));
        return std::nullopt;
    }

    Customer customer;
    customer._id = SodiumAuth::generate_id();
    customer.name = result->name;
    customer.email = result->email;
    customer.phone = result->phone;
    customer.address = result->address;
    customer.password = SodiumAuth::hash_password(result->password);

    if (!CustomerTable::insert(db, customer))
    {
        std::println(std::cerr, "Registration failed (email already registered?)");
        return std::nullopt;
    }
    Account account;
    int account_choice{};
    account._id = SodiumAuth::generate_id();
    account.account_number = SodiumAuth::generateAccountNumber();
    account.account_status = AccountStatus::Active;
    account.linkedCustomer = customer._id;
    std::println("Select Your Account Type: \n{}) Current\n{}) Saving\n{}) Business",
                 static_cast<int>(AccountType::Current),
                 static_cast<int>(AccountType::Saving),
                 static_cast<int>(AccountType::Business));
    std::cout << "> ";
    std::cin >> account_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (account_choice >= 0 && account_choice <= 2)
    {
        account.account_type = static_cast<AccountType>(account_choice);
    }
    else
    {
        std::println("Invalid selection. Defaulting to Current.");
        account.account_type = AccountType::Current;
    }
    if (!AccountTable::insert(db, account))
    {
        std::println(std::cerr, "Account creation failed...");
        return std::nullopt;
    }
    std::println("Registered successfully: [{}] <{}>", customer.name, customer.email);
    printer.print_styled_banner("Login now!!");
    return login();
}
