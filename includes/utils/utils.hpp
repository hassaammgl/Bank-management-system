#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <expected>
#include <print>

inline bool isValidEmail(const std::string &email)
{
    static const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

inline bool isValidPhone(const std::string &phone)
{
    static const std::regex pattern(R"(^(\+92|0)?3\d{2}-?\d{7}$)");
    return std::regex_match(phone, pattern);
}

inline bool isValidCNIC(const std::string &cnic)
{
    static const std::regex pattern(R"(^\d{5}-?\d{7}-?\d{1}$)");
    return std::regex_match(cnic, pattern);
}