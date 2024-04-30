#include "cache.h"

namespace dpp
{
    namespace utility
    {
        std::optional<dpp::guild_member> find_guild_member_opt(const dpp::snowflake guild_id, const dpp::snowflake user_id)
        {
            try
            {
                return dpp::find_guild_member(guild_id, user_id);
            }
            catch (const dpp::cache_exception& e)
            {
                return std::nullopt;
            }
        }
    }
}
