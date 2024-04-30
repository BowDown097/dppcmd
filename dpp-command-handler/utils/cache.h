#ifndef CACHE_H
#define CACHE_H
#include <dpp/guild.h>

namespace dpp
{
    class snowflake;

    namespace utility
    {
        std::optional<dpp::guild_member> find_guild_member_opt(const dpp::snowflake guild_id, const dpp::snowflake user_id);
    }
}

#endif // CACHE_H
