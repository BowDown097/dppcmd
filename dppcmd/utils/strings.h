#pragma once
#include <string_view>

namespace dppcmd
{
    namespace utility
    {
        std::string demangle(std::string_view name);
        bool iequals(std::string_view s1, std::string_view s2);
        bool sequals(std::string_view s1, std::string_view s2, bool caseSensitive);
    }
}
