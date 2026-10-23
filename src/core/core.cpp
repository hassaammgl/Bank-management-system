#include <core/core.hpp>
#include <core/db.hpp>
#include <iostream>
#include <print>

Core::Core(sqlite3 *db_) : db(db_), auth(db_), _dashboard(db_){}

void Core::run()
{
    while (true)
    {
        if (!session)
        {
            // Auth phase: login/register success → session set, Exit → app band
            auto customer = auth.run();
            if (!customer)
                return;

            auto account = AccountTable::find_by_customer(db, customer->_id);
            if (account)
            {
                session = Session(*customer, *account);
            }
            else
            {
                std::println(std::cerr, "No account found for {}. Please contact support.",
                             customer->email);
                return;
            }
        }
        else
        {
            // Dashboard phase: Logout → wapas Auth, Exit → app band
            auto action = _dashboard.run(*session);
            if (action == Dashboard::Action::Exit)
                return;
            session.reset();   // Logout
        }
    }
}
