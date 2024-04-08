#ifndef JOIN_H
#define JOIN_H
#include <numeric>
#include <string>

namespace cmdhndlrutils
{
    std::string join(std::ranges::sized_range auto&& range, const std::string& delim)
    {
        size_t size = std::ranges::size(range);
        if (size > 1)
        {
            return std::accumulate(
                std::next(std::ranges::cbegin(range)), std::ranges::cend(range),
                lexical_cast<std::string>(*std::ranges::cbegin(range)),
                [&delim](std::string a, auto& b) { return a + delim + lexical_cast<std::string>(b); }
            );
        }
        else if (size == 1)
        {
            return lexical_cast<std::string>(*std::ranges::cbegin(range));
        }

        return "";
    }

    std::string rjoin(std::ranges::sized_range auto&& range, const std::string& delim)
    {
        size_t size = std::ranges::size(range);
        if (size > 1)
        {
            return std::accumulate(
                std::next(std::ranges::crbegin(range)), std::ranges::crend(range),
                lexical_cast<std::string>(*std::ranges::crbegin(range)),
                [&delim](std::string a, auto& b) { return a + delim + lexical_cast<std::string>(b); }
            );
        }
        else if (size == 1)
        {
            return lexical_cast<std::string>(*std::ranges::cbegin(range));
        }

        return "";
    }
}

#endif // JOIN_H
