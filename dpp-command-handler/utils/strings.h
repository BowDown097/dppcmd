#pragma once
#include <string_view>

namespace dpp
{
    namespace utility
    {
        bool iequals(std::string_view s1, std::string_view s2);
        bool sequals(std::string_view s1, std::string_view s2, bool caseSensitive);
    }
}
