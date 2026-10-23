#include <core/auth.hpp>
#include <utils/utils.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <stdexcept>
#include <random>
#include <sodium.h>

namespace SodiumAuth
{
    std::string generateAccountNumber(int length = 10)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);

        std::string accountNumber = "";
        for (int i = 0; i < length; ++i)
        {
            accountNumber += std::to_string(dis(gen));
        }
        return accountNumber;
    }

    std::string generate_id()
    {
        unsigned char raw[12];
        randombytes_buf(raw, sizeof raw);

        char hex[sizeof raw * 2 + 1];
        sodium_bin2hex(hex, sizeof hex, raw, sizeof raw);
        return hex;
    }

    std::string hash_password(const std::string &password)
    {
        char hash[crypto_pwhash_STRBYTES];
        if (crypto_pwhash_str(hash,
                              password.c_str(),
                              static_cast<unsigned long long>(password.size()),
                              crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
        {
            throw std::runtime_error("password hashing failed");
        }
        return hash;
    }

    bool verify_password(const std::string &hash, const std::string &password)
    {
        return crypto_pwhash_str_verify(hash.c_str(),
                                        password.c_str(),
                                        static_cast<unsigned long long>(password.size())) == 0;
    }
}