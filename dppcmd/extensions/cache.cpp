#include "cache.h"
#include <dpp/cache.h>
#include <dpp/guild.h>

namespace dppcmd
{
    dpp::channel* find_guild_channel(const dpp::snowflake guild_id, const dpp::snowflake channel_id)
    {
        if (dpp::guild* guild = find_guild(guild_id); std::ranges::find(guild->channels, channel_id) != guild->channels.end())
            if (dpp::channel* channel = find_channel(channel_id))
                return channel;
        return nullptr;
    }

    std::optional<dpp::guild_member> find_guild_member_opt(const dpp::snowflake guild_id, const dpp::snowflake user_id)
    {
        try
        {
            return find_guild_member(guild_id, user_id);
        }
        catch (const dpp::cache_exception& e)
        {
            return std::nullopt;
        }
    }

    dpp::role* find_guild_role(const dpp::snowflake guild_id, const dpp::snowflake role_id)
    {
        if (dpp::guild* guild = find_guild(guild_id); std::ranges::find(guild->roles, role_id) != guild->roles.end())
            if (dpp::role* role = find_role(role_id))
                return role;
        return nullptr;
    }

    dpp::user* find_guild_user(const dpp::snowflake guild_id, const dpp::snowflake user_id)
    {
        if (const dpp::guild* guild = find_guild(guild_id))
            for (const auto& [id, member] : guild->members)
                if (id == user_id)
                    return member.get_user();
        return nullptr;
    }
}
