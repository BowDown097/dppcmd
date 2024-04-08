#include "strings.h"
#include <algorithm>

namespace cmdhndlrutils
{
    bool iequals(const std::string& s1, const std::string& s2)
    {
        return std::ranges::equal(s1, s2, [](char a, char b) { return tolower(a) == tolower(b); });
    }

    bool sequals(const std::string& s1, const std::string& s2, bool caseSensitive)
    {
        return caseSensitive ? s1 == s2 : iequals(s1, s2);
    }
}
