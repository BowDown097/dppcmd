#ifndef MENTIONS_H
#define MENTIONS_H
#include <cstdint>
#include <string_view>

namespace dpp
{
    namespace utility
    {
        uint64_t parse_channel_mention(std::string_view str);
        uint64_t parse_role_mention(std::string_view str);
        uint64_t parse_user_mention(std::string_view str);
    }
}

#endif // MENTIONS_H
