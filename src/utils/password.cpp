#include <utils/password.hpp>
#include <sodium.h>
#include <stdexcept>

bool Password::init()
{
    return sodium_init() >= 0;
}

std::string Password::hash(const std::string &plain)
{
    char out[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(out, plain.c_str(), plain.size(),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        throw std::runtime_error("password hashing failed");
    }
    return std::string(out);
}

bool Password::verify(const std::string &hash, const std::string &plain)
{
    return crypto_pwhash_str_verify(hash.c_str(), plain.c_str(), plain.size()) == 0;
}
// Customer c;
// c.name = "Hassaam";
// c.email = "h@mail.com";
// c.password = Password::hash(plainPassword); // hash karke hi struct mein daalo
// CustomerTable::insert(db, c);