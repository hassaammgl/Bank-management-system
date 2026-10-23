#include <algorithm>
#include <cctype>
#include <cstddef>
#include <print>
#include <string>
#include <string_view>

#include <utils/banner.hpp>

namespace
{
    std::size_t display_width(std::string_view text)
    {
        std::size_t width = 0;
        for (char ch : text)
        {
            const unsigned char byte = static_cast<unsigned char>(ch);
            if ((byte & 0xC0U) != 0x80U)
            {
                ++width;
            }
        }
        return width;
    }

    std::string repeat(std::string_view unit, std::size_t count)
    {
        std::string out;
        out.reserve(unit.size() * count);
        for (std::size_t i = 0; i < count; ++i)
        {
            out += unit;
        }
        return out;
    }

    std::string gradient(std::size_t index, std::size_t count)
    {
        const double t = count > 1 ? static_cast<double>(index) / static_cast<double>(count - 1) : 0.0;
        const int red = static_cast<int>(t * 180.0);
        const int green = static_cast<int>((1.0 - t) * 200.0);
        constexpr int blue = 255;
        return "\033[38;2;" + std::to_string(red) + ";" +
               std::to_string(green) + ";" + std::to_string(blue) + "m";
    }
}

void BannerPrinter::init_font()
{
    font_map['A'] = {" █████╗ ", "██╔══██╗", "███████║", "██╔══██║", "██║  ██║"};
    font_map['B'] = {"██████╗ ", "██╔══██╗", "██████╔╝", "██╔══██╗", "██████╔╝"};
    font_map['C'] = {" ██████╗", "██╔════╝", "██║     ", "██║     ", "╚██████╗"};
    font_map['D'] = {"██████╗ ", "██╔══██╗", "██║  ██║", "██║  ██║", "██████╔╝"};
    font_map['E'] = {"████████╗", "██╔═════╝", "█████╗  ", "██╔══╝  ", "████████╗"};
    font_map['F'] = {"███████╗", "██╔════╝", "█████╗  ", "██╔══╝  ", "██║     "};
    font_map['G'] = {" ██████╗", "██╔════╝", "██║  ███╗", "██║   ██║", "╚██████╔╝"};
    font_map['H'] = {"██╗  ██╗", "██║  ██║", "███████║", "██╔══██║", "██║  ██║"};
    font_map['I'] = {"██╗", "██║", "██║", "██║", "██║"};
    font_map['J'] = {"     ██╗", "     ██║", "     ██║", "██   ██║", "╚█████╔╝"};
    font_map['K'] = {"██╗  ██╗", "██║ ██╔╝", "█████═╝ ", "██╔═██╗ ", "██║  ██╗"};
    font_map['L'] = {"██╗     ", "██║     ", "██║     ", "██║     ", "███████╗"};
    font_map['M'] = {"██╗   ██╗", "████ ████║", "██╔████╔██║", "██║╚██╔╝██║", "██║ ╚═╝ ██║"};
    font_map['N'] = {"██████╗ ", "██╔══██╗", "██║  ██║", "██║  ██║", "██║  ██║"};
    font_map['O'] = {" ██████╗ ", "██╔══██╗", "██║  ██║", "██║  ██║", "╚██████╔╝"};
    font_map['P'] = {"██████╗ ", "██╔══██╗", "██████╔╝", "██╔═══╝ ", "██║     "};
    font_map['Q'] = {" ██████╗ ", "██╔══██╗", "██║  ██║", "██║████║", "╚██████╔╝"};
    font_map['R'] = {"██████╗ ", "██╔══██╗", "██████╔╝", "██╔══██╗", "██║  ██║"};
    font_map['S'] = {" ██████╗", "██╔════╝", "╚█████╗ ", " ╚═══██╗", "██████╔╝"};
    font_map['T'] = {"████████╗", "╚══██╔══╝", "   ██║   ", "   ██║   ", "   ██║   "};
    font_map['U'] = {"██╗  ██╗", "██║  ██║", "██║  ██║", "██║  ██║", "╚██████╔╝"};
    font_map['V'] = {"██╗   ██╗", "██║   ██║", "██║   ██║", "╚██╗ ██╔╝", " ╚████╔╝ "};
    font_map['W'] = {"██╗   ██╗", "██║   ██║", "██║ █╗ ██║", "██║███╗██║", "╚███╔███╔╝"};
    font_map['X'] = {"██╗  ██╗", "╚██╗██╔╝", " ╚███╔╝ ", " ██╔██╗ ", "██╔╝ ██╗"};
    font_map['Y'] = {"██╗   ██╗", "╚██╗ ██╔╝", " ╚████╔╝ ", "  ╚██╔╝  ", "   ██║   "};
    font_map['Z'] = {"████████╗", "╚══██╔══╝", "  ██╔╝  ", " ██╔╝   ", "████████╗"};

    font_map['0'] = {" ██████╗ ", "██╔═████╗", "██║██╔██║", "████╔╝██║", "╚██████╔╝"};
    font_map['1'] = {"██╗", "██║", "██║", "██║", "██║"};
    font_map['2'] = {"██████╗ ", "╚════██╗", " █████╔╝", "██╔═══╝ ", "███████╗"};
    font_map['3'] = {"██████╗ ", "╚════██╗", " █████╔╝", " ╚═══██╗", "██████╔╝"};
    font_map['4'] = {"██╗  ██╗", "██║  ██║", "███████║", "╚════██║", "     ██║"};
    font_map['5'] = {"███████╗", "██╔════╝", "███████╗", "╚════██║", "███████╔╝"};
    font_map['6'] = {" ██████╗", "██╔════╝", "███████╗", "██╔══██║", "╚██████╔╝"};
    font_map['7'] = {"████████╗", "╚══██╔══╝", "   ██║   ", "   ██║   ", "   ██║   "};
    font_map['8'] = {" ██████╗ ", "██╔══██╗", "╚██████╔╝", "██╔══██╗", "╚██████╔╝"};
    font_map['9'] = {" ██████╗ ", "██╔══██╗", "╚██████║", " ╚═══██║", " ██████╔╝"};

    font_map[' '] = {"   ", "   ", "   ", "   ", "   "};
    font_map['!'] = {"██╗", "██║", "██║", "╚═╝", "██╗"};
    font_map['?'] = {"██████╗ ", "╚════██╗", "  ████╔╝", "  ╚═══╝ ", "  ██╗   "};
}

BannerPrinter::BannerPrinter()
{
    init_font();
}

void BannerPrinter::print_styled_banner(const std::string &text)
{
    constexpr std::size_t height = 5;
    std::vector<std::string> glyph_lines(height, "");

    const auto fallback = font_map.find('?');

    for (char c : text)
    {
        const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        auto it = font_map.find(upper);
        if (it == font_map.end())
        {
            it = fallback;
        }
        if (it == font_map.end())
        {
            continue;
        }

        for (std::size_t i = 0; i < height; ++i)
        {
            glyph_lines[i] += it->second[i];
        }
    }

    std::size_t text_width = 0;
    for (const auto &line : glyph_lines)
    {
        text_width = std::max(text_width, display_width(line));
    }

    constexpr std::size_t pad = 2;
    const std::size_t inner = text_width + pad * 2;

    constexpr std::string_view RESET = "\033[0m";
    constexpr std::string_view FRAME = "\033[38;2;120;120;170m";

    const std::string horizontal = repeat("═", inner);
    const std::string blank(inner, ' ');

    std::println("{}╔{}╗{}", FRAME, horizontal, RESET);
    std::println("{}║{}║{}", FRAME, blank, RESET);

    for (std::size_t i = 0; i < height; ++i)
    {
        const std::string gap(text_width - display_width(glyph_lines[i]), ' ');
        const std::string color = gradient(i, height);
        std::println("{}{}  {}{}  {}{}", color, "║", glyph_lines[i], gap, "║", RESET);
    }

    std::println("{}║{}║{}", FRAME, blank, RESET);
    std::println("{}╚{}╝{}", FRAME, horizontal, RESET);
}
