#ifndef MENTIONS_H
#define MENTIONS_H
#include <cstdint>
#include <string>

namespace cmdhndlrutils
{
    namespace mentions
    {
        uint64_t parseChannel(const std::string& str);
        uint64_t parseRole(const std::string& str);
        uint64_t parseUser(const std::string& str);
    }
}

#endif // MENTIONS_H
