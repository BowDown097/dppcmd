#include "strings.h"
#include <algorithm>
#include <cctype>

namespace cmdhndlrutils
{
    bool iequals(std::string_view s1, std::string_view s2)
    {
        return std::ranges::equal(s1, s2, [](char a, char b) { return tolower(a) == tolower(b); });
    }

    bool sequals(std::string_view s1, std::string_view s2, bool caseSensitive)
    {
        return caseSensitive ? s1 == s2 : iequals(s1, s2);
    }
}
