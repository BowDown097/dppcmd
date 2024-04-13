#include "usertypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <dpp/user.h>

TypeReaderResult UserTypeReader::read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input)
{
    // by mention (1.0)
    if (uint64_t id = dpp::utility::parse_user_mention(input))
        if (dpp::user* user = dpp::find_user(id))
            addResult(user);

    // by ID (0.9)
    if (uint64_t id = dpp::utility::lexical_cast<uint64_t>(input, false))
        if (dpp::user* user = dpp::find_user(id))
            addResult(user, 0.9f);

    // by username + discriminator (0.7-0.8)
    if (size_t index = input.find_last_of('#'); index != std::string::npos)
    {
        std::string_view username = input.substr(0, index);
        if (uint16_t discrim = dpp::utility::lexical_cast<uint16_t>(input, false))
        {
            dpp::cache<dpp::user>* userCache = dpp::get_user_cache();
            std::shared_lock l(userCache->get_mutex());

            std::unordered_map<dpp::snowflake, dpp::user*> container = userCache->get_container();
            auto it = std::ranges::find_if(container, [&discrim, &username](const auto& pair) {
                return pair.second->discriminator == discrim && dpp::utility::iequals(pair.second->username, username);
            });

            if (it != container.end())
            {
                dpp::user* matchingUser = it->second;
                addResult(matchingUser, matchingUser->username == username ? 0.8f : 0.7f);
            }
        }
    }

    // by global (display) name (0.5-0.6)
    {
        dpp::cache<dpp::user>* userCache = dpp::get_user_cache();
        std::shared_lock l(userCache->get_mutex());

        for (const auto& [_, user] : userCache->get_container())
            if (dpp::utility::iequals(user->global_name, input))
                addResult(user, user->global_name == input ? 0.6f : 0.5f);
    }

    // by username (0.5-0.6)
    {
        dpp::cache<dpp::user>* userCache = dpp::get_user_cache();
        std::shared_lock l(userCache->get_mutex());

        for (const auto& [_, user] : userCache->get_container())
            if (dpp::utility::iequals(user->username, input))
                addResult(user, user->username == input ? 0.6f : 0.5f);
    }

    // by nickname (0.5-0.6)
    {
        dpp::cache<dpp::guild>* guildCache = dpp::get_guild_cache();
        std::shared_lock l(guildCache->get_mutex());

        for (const auto& [_, guild] : guildCache->get_container())
            for (const auto& [_, guildUser] : guild->members)
                if (dpp::utility::iequals(guildUser.get_nickname(), input))
                    if (dpp::user* user = guildUser.get_user())
                        addResult(user, guildUser.get_nickname() == input ? 0.6f : 0.5f);
    }

    if (hasResult())
        return TypeReaderResult::fromSuccess();
    return TypeReaderResult::fromError(CommandError::ObjectNotFound, "User not found.");
}
