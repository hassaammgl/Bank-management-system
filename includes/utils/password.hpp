#pragma once
#include <string>

namespace Password
{
    bool init();                                                    // main mein ek baar
    std::string hash(const std::string &plain);                     // signup pe
    bool verify(const std::string &hash, const std::string &plain); // login pe
}