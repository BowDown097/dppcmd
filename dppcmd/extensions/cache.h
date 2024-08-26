#pragma once
#include <optional>

namespace dpp { class channel; class guild_member; class role; class snowflake; class user; }

namespace dppcmd
{
    dpp::channel* find_guild_channel(const dpp::snowflake guild_id, const dpp::snowflake channel_id);
    std::optional<dpp::guild_member> find_guild_member_opt(const dpp::snowflake guild_id, const dpp::snowflake user_id);
    dpp::role* find_guild_role(const dpp::snowflake guild_id, const dpp::snowflake role_id);
    dpp::user* find_guild_user(const dpp::snowflake guild_id, const dpp::snowflake user_id);
}
