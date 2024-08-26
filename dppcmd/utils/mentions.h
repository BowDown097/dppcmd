#pragma once
#include <cstdint>
#include <string_view>

namespace dppcmd
{
    namespace utility
    {
        uint64_t parse_channel_mention(std::string_view str);
        uint64_t parse_role_mention(std::string_view str);
        uint64_t parse_user_mention(std::string_view str);
    }
}
