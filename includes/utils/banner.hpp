#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class BannerPrinter
{
private:
    std::unordered_map<char, std::vector<std::string>> font_map;

    void init_font();

public:
    BannerPrinter();

    void print_styled_banner(const std::string &text);
};
