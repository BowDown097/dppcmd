#include "cache.h"
#include <dpp/cache.h>
#include <dpp/guild.h>

namespace dpp
{
    channel* find_guild_channel(const snowflake guild_id, const snowflake channel_id)
    {
        if (guild* guild = find_guild(guild_id); std::ranges::find(guild->channels, channel_id) != guild->channels.end())
            if (channel* channel = find_channel(channel_id))
                return channel;
        return nullptr;
    }

    std::optional<guild_member> find_guild_member_opt(const snowflake guild_id, const snowflake user_id)
    {
        try
        {
            return find_guild_member(guild_id, user_id);
        }
        catch (const cache_exception& e)
        {
            return std::nullopt;
        }
    }

    role* find_guild_role(const snowflake guild_id, const snowflake role_id)
    {
        if (guild* guild = find_guild(guild_id); std::ranges::find(guild->roles, role_id) != guild->roles.end())
            if (role* role = find_role(role_id))
                return role;
        return nullptr;
    }

    user* find_guild_user(const snowflake guild_id, const snowflake user_id)
    {
        if (guild* guild = find_guild(guild_id))
            for (const auto& [id, member] : guild->members)
                if (id == user_id)
                    return member.get_user();
        return nullptr;
    }
}
