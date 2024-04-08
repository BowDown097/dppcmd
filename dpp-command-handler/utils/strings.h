#ifndef STRINGS_H
#define STRINGS_H
#include <string>

namespace cmdhndlrutils
{
    bool iequals(const std::string& s1, const std::string& s2);
    bool sequals(const std::string& s1, const std::string& s2, bool caseSensitive);
}

#endif // STRINGS_H
