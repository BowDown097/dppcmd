#include "usertypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <dpp/user.h>

namespace dpp
{
    type_reader_result user_in::read(cluster* cluster, const message_create_t* context, std::string_view input)
    {
        // by mention (1.0)
        if (uint64_t id = utility::parse_user_mention(input))
            if (user* user = find_user(id))
                add_result(user);

        // by ID (0.9)
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (user* user = find_user(id))
                add_result(user, 0.9f);

        // by username + discriminator (0.7-0.8)
        if (size_t index = input.find_last_of('#'); index != std::string::npos)
        {
            std::string_view username = input.substr(0, index);
            if (uint16_t discrim = utility::lexical_cast<uint16_t>(input, false))
            {
                cache<user>* userCache = get_user_cache();
                std::shared_lock l(userCache->get_mutex());

                std::unordered_map<snowflake, user*> container = userCache->get_container();
                auto it = std::ranges::find_if(container, [&discrim, &username](const auto& pair) {
                    return pair.second->discriminator == discrim && utility::iequals(pair.second->username, username);
                });

                if (it != container.end())
                {
                    user* matchingUser = it->second;
                    add_result(matchingUser, matchingUser->username == username ? 0.8f : 0.7f);
                }
            }
        }

        // by global (display) name (0.5-0.6)
        {
            cache<user>* userCache = get_user_cache();
            std::shared_lock l(userCache->get_mutex());

            for (const auto& [_, user] : userCache->get_container())
                if (utility::iequals(user->global_name, input))
                    add_result(user, user->global_name == input ? 0.6f : 0.5f);
        }

        // by username (0.5-0.6)
        {
            cache<user>* userCache = get_user_cache();
            std::shared_lock l(userCache->get_mutex());

            for (const auto& [_, user] : userCache->get_container())
                if (utility::iequals(user->username, input))
                    add_result(user, user->username == input ? 0.6f : 0.5f);
        }

        // by nickname (0.5-0.6)
        {
            cache<guild>* guildCache = get_guild_cache();
            std::shared_lock l(guildCache->get_mutex());

            for (const auto& [_, guild] : guildCache->get_container())
                for (const auto& [_, guildUser] : guild->members)
                    if (utility::iequals(guildUser.get_nickname(), input))
                        if (user* user = guildUser.get_user())
                            add_result(user, guildUser.get_nickname() == input ? 0.6f : 0.5f);
        }

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "User not found.");
    }
}
