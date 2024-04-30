#include "channeltypereader.h"
#include "dpp-command-handler/extensions/cache.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/channel.h>
#include <dpp/dispatcher.h>
#include <dpp/guild.h>

namespace dpp
{
    type_reader_result channel_in::read(cluster* cluster, const message_create_t* context, std::string_view input)
    {
        add_results_by_mention(context->msg.guild_id, input); // weight: 1.0
        add_results_by_id(context->msg.guild_id, input); // weight: 0.9
        add_results_by_name(context->msg.guild_id, input); // weight: 0.7-0.8

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "Channel not found.");
    }

    void channel_in::add_results_by_id(const snowflake guild_id, std::string_view input)
    {
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (channel* channel = find_guild_channel(guild_id, id))
                add_result(channel, 0.9f);
    }

    void channel_in::add_results_by_mention(const snowflake guild_id, std::string_view input)
    {
        if (snowflake id = utility::parse_channel_mention(input))
            if (channel* channel = find_guild_channel(guild_id, id))
                add_result(channel);
    }

    void channel_in::add_results_by_name(const snowflake guild_id, std::string_view input)
    {
        if (guild* guild = find_guild(guild_id))
            for (snowflake channel_id : guild->channels)
                if (channel* channel = find_channel(channel_id); utility::iequals(channel->name, input))
                    return add_result(channel, channel->name == input ? 0.8f : 0.7f);
    }
}
