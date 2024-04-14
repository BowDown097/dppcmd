#ifndef JOIN_H
#define JOIN_H
#include <numeric>
#include <string>

namespace dpp
{
    namespace utility
    {
        std::string join(std::ranges::sized_range auto&& range, std::string_view delim)
        {
            size_t size = std::ranges::size(range);
            if (size > 1)
            {
                return std::accumulate(
                    std::next(std::ranges::cbegin(range)), std::ranges::cend(range),
                    lexical_cast<std::string>(*std::ranges::cbegin(range)),
                    [&delim](std::string a, const auto& b) { return a + std::string(delim) + lexical_cast<std::string>(b); }
                );
            }
            else if (size == 1)
            {
                return lexical_cast<std::string>(*std::ranges::cbegin(range));
            }

            return "";
        }
    }
}

#endif // JOIN_H
