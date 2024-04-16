#ifndef JOIN_H
#define JOIN_H
#include <string>

namespace dpp
{
    namespace utility
    {
        std::string join(std::ranges::sized_range auto&& range, const std::string& delim)
        {
            size_t size = std::ranges::size(range);
            if (size > 1)
            {
                std::string init = lexical_cast<std::string>(*std::ranges::cbegin(range));
                for (auto it = std::next(std::ranges::cbegin(range)); it != std::ranges::cend(range); ++it)
                {
                    init += delim;
                    init += lexical_cast<std::string>(*it);
                }
                return init;
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
