#include "usertypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/dispatcher.h>

namespace dpp
{
    type_reader_result user_in::read(cluster* cluster, const message_create_t* context, std::string_view input)
    {
        add_results_by_mention(input); // weight: 1.0
        add_results_by_id(input); // weight: 0.9
        add_results_by_username_and_discrim(input); // weight: 0.7-0.8
        add_results_by_global_name(input); // weight: 0.5-0.6
        add_results_by_username(input); // weight: 0.5-0.6
        add_results_by_nickname(context->msg.guild_id, input); // weight: 0.5-0.6

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "User not found.");
    }

    void user_in::add_results_by_global_name(std::string_view input)
    {
        cache<user>* userCache = get_user_cache();
        std::shared_lock l(userCache->get_mutex());

        for (const auto& [_, user] : userCache->get_container())
            if (utility::iequals(user->global_name, input))
                add_result(user, user->global_name == input ? 0.6f : 0.5f);
    }

    void user_in::add_results_by_id(std::string_view input)
    {
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (user* user = find_user(id))
                add_result(user, 0.9f);
    }

    void user_in::add_results_by_mention(std::string_view input)
    {
        if (uint64_t id = utility::parse_user_mention(input))
            if (user* user = find_user(id))
                add_result(user);
    }

    void user_in::add_results_by_nickname(const dpp::snowflake guild_id, std::string_view input)
    {
        cache<guild>* guildCache = get_guild_cache();
        std::shared_lock l(guildCache->get_mutex());

        for (const auto& [_, guild] : guildCache->get_container())
            if (guild->id == guild_id)
                for (const auto& [_, guildUser] : guild->members)
                    if (std::string nickname = guildUser.get_nickname(); utility::iequals(nickname, input))
                        if (user* user = guildUser.get_user())
                            add_result(user, nickname == input ? 0.6f : 0.5f);
    }

    void user_in::add_results_by_username(std::string_view input)
    {
        cache<user>* userCache = get_user_cache();
        std::shared_lock l(userCache->get_mutex());

        for (const auto& [_, user] : userCache->get_container())
            if (utility::iequals(user->username, input))
                add_result(user, user->username == input ? 0.6f : 0.5f);
    }

    void user_in::add_results_by_username_and_discrim(std::string_view input)
    {
        if (size_t index = input.find_last_of('#'); index != std::string_view::npos)
        {
            std::string_view username = input.substr(0, index);
            if (uint16_t discrim = utility::lexical_cast<uint16_t>(input.substr(index + 1), false))
            {
                cache<user>* userCache = get_user_cache();
                std::shared_lock l(userCache->get_mutex());

                for (const auto& [_, user] : userCache->get_container())
                    if (user->discriminator == discrim && utility::iequals(user->username, username))
                        add_result(user, user->username == username ? 0.8f : 0.7f);
            }
        }
    }
}
