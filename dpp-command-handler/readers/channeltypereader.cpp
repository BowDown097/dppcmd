#include "channeltypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/channel.h>

namespace dpp
{
    type_reader_result channel_in::read(cluster* cluster, const message_create_t* context, std::string_view input)
    {
        add_results_by_mention(input); // weight: 1.0
        add_results_by_id(input); // weight: 0.9
        add_results_by_name(input); // weight: 0.7-0.8

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "Channel not found.");
    }

    void channel_in::add_results_by_id(std::string_view input)
    {
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (channel* channel = find_channel(id))
                add_result(channel, 0.9f);
    }

    void channel_in::add_results_by_mention(std::string_view input)
    {
        if (uint64_t id = utility::parse_channel_mention(input))
            if (channel* channel = find_channel(id))
                add_result(channel);
    }

    void channel_in::add_results_by_name(std::string_view input)
    {
        cache<channel>* channelCache = get_channel_cache();
        std::shared_lock l(channelCache->get_mutex());

        for (const auto& [_, channel] : channelCache->get_container())
            if (utility::iequals(channel->name, input))
                add_result(channel, channel->name == input ? 0.8f : 0.7f);
    }
}
