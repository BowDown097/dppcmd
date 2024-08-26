#include "channeltypereader.h"
#include "dppcmd/extensions/cache.h"
#include "dppcmd/utils/lexical_cast.h"
#include "dppcmd/utils/mentions.h"
#include "dppcmd/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/channel.h>
#include <dpp/dispatcher.h>
#include <dpp/guild.h>

namespace dppcmd
{
    type_reader_result channel_in::read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input)
    {
        add_results_by_mention(context->msg.guild_id, input); // weight: 1.0
        add_results_by_id(context->msg.guild_id, input); // weight: 0.9
        add_results_by_name(context->msg.guild_id, input); // weight: 0.7-0.8

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "Channel not found.");
    }

    void channel_in::add_results_by_id(const dpp::snowflake guild_id, std::string_view input)
    {
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (dpp::channel* channel = find_guild_channel(guild_id, id))
                if (id_set.insert(id).second)
                    add_result(channel, 0.9f);
    }

    void channel_in::add_results_by_mention(const dpp::snowflake guild_id, std::string_view input)
    {
        if (dpp::snowflake id = utility::parse_channel_mention(input))
            if (dpp::channel* channel = find_guild_channel(guild_id, id))
                if (id_set.insert(id).second)
                    add_result(channel);
    }

    void channel_in::add_results_by_name(const dpp::snowflake guild_id, std::string_view input)
    {
        if (const dpp::guild* guild = find_guild(guild_id))
            for (dpp::snowflake channel_id : guild->channels)
                if (dpp::channel* channel = find_channel(channel_id); utility::iequals(channel->name, input))
                    if (id_set.insert(channel_id).second)
                        return add_result(channel, channel->name == input ? 0.8f : 0.7f);
    }
}
