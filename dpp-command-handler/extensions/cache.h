#pragma once
#include <optional>

namespace dpp
{
    class channel;
    class guild_member;
    class role;
    class snowflake;
    class user;

    channel* find_guild_channel(const snowflake guild_id, const snowflake channel_id);
    std::optional<guild_member> find_guild_member_opt(const snowflake guild_id, const snowflake user_id);
    role* find_guild_role(const snowflake guild_id, const snowflake role_id);
    user* find_guild_user(const snowflake guild_id, const snowflake user_id);
}
