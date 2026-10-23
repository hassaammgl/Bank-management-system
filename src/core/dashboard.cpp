#include <core/dashboard.hpp>
#include <array>
#include <iostream>
#include <limits>

namespace
{
    constexpr std::array<Account::Money, 5> kPresets{500, 1000, 3000, 5000, 10000};

    std::string_view to_string(AccountType t)
    {
        switch (t)
        {
        case AccountType::Current:
            return "Current";
        case AccountType::Saving:
            return "Saving";
        case AccountType::Business:
            return "Business";
        }
        return "Unknown";
    }

    void flush_cin()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
} // namespace

Dashboard::Dashboard(sqlite3 *db_) : db(db_) {}

void Dashboard::showMenu()
{
    std::println("1) Check Balance.");
    std::println("2) Deposit Money.");
    std::println("3) Withdraw Money.");
    std::println("4) Transfer Money.");
    std::println("5) Card Services.");
    std::println("6) Loan Services.");
    std::println("7) View Transaction History.");
    std::println("8) Logout");
    std::println("0) Exit");
    std::cout << "> ";
}

void Dashboard::not_implemented(std::string_view what, const Session &session)
{
    std::println("{} -- coming soon. ({} | {})", what,
                 session.get_customer_name(), session.get_account_number());
}

Dashboard::Action Dashboard::run(Session &session)
{
    printer.print_styled_banner("DASHBOARD!!");
    while (session.is_active())
    {
        showMenu();

        int choice{};
        if (!(std::cin >> choice))
        {
            flush_cin();
            std::println(std::cerr, "Invalid input, try again.");
            continue;
        }
        flush_cin();

        switch (choice)
        {
        case 1:
            check_balance(session);
            break;
        case 2:
            deposit_money(session);
            break;
        case 3:
            not_implemented("Withdraw Money", session);
            break;
        case 4:
            not_implemented("Transfer Money", session);
            break;
        case 5:
            not_implemented("Card Services", session);
            break;
        case 6:
            not_implemented("Loan Services", session);
            break;
        case 7:
            not_implemented("Transaction History", session);
            break;
        case 8:
            session.end();
            return Action::Logout;
        case 0:
            return Action::Exit;
        default:
            std::println(std::cerr, "Invalid choice!");
        }
    }
    return Action::Logout;
}

void Dashboard::check_balance(const Session &session)
{
    const auto &customer = session.get_customer();
    const auto &account = session.get_account();

    std::println("Account Holder : {}", customer.name);
    std::println("Account Number : {}", account.account_number);
    std::println("Account Type   : {}", to_string(account.account_type));
    std::println("Balance        : {}", account.balance);
}

void Dashboard::deposit_money(Session &session)
{
    const auto &account = session.get_account();

    std::println("Account Holder : {}", session.get_customer().name);
    std::println("Account Type   : {}", to_string(account.account_type));
    std::println("Select Amount:");
    for (std::size_t i = 0; i < kPresets.size(); ++i)
        std::println("{}) {}", i + 1, kPresets[i]);

    const int customOption = static_cast<int>(kPresets.size()) + 1;
    std::println("{}) Custom", customOption);
    std::cout << "> ";

    int choice{};
    if (!(std::cin >> choice))
    {
        flush_cin();
        std::println("Invalid input");
        return;
    }
    flush_cin();

    Account::Money amount{};
    if (choice >= 1 && choice <= static_cast<int>(kPresets.size()))
    {
        amount = kPresets[static_cast<std::size_t>(choice - 1)];
    }
    else if (choice == customOption)
    {
        std::cout << "Enter amount: ";
        if (!(std::cin >> amount) || amount <= 0)
        {
            flush_cin();
            std::println("Invalid amount");
            return;
        }
        flush_cin();
    }
    else
    {
        std::println("Invalid option");
        return;
    }

    auto updated = AccountTable::update_balance(db, account.linkedCustomer, amount);
    if (!updated)
    {
        std::println(std::cerr, "Deposit failed.");
        return;
    }

    session.set_account(*updated);
    std::println("Deposited {}. New balance: {}", amount, updated->balance);
}