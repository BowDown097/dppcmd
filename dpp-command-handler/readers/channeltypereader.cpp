#include "channeltypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/channel.h>

TypeReaderResult ChannelTypeReader::read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input)
{
    // by mention (1.0)
    if (uint64_t id = dpp::utility::parse_channel_mention(input))
        if (dpp::channel* channel = dpp::find_channel(id))
            addResult(channel);

    // by ID (0.9)
    if (uint64_t id = dpp::utility::lexical_cast<uint64_t>(input, false))
        if (dpp::channel* channel = dpp::find_channel(id))
            addResult(channel, 0.9f);

    // by name (0.7-0.8)
    {
        dpp::cache<dpp::channel>* channelCache = dpp::get_channel_cache();
        std::shared_lock l(channelCache->get_mutex());

        for (const auto& [_, channel] : channelCache->get_container())
            if (dpp::utility::iequals(channel->name, input))
                addResult(channel, channel->name == input ? 0.8f : 0.7f);
    }

    if (hasResult())
        return TypeReaderResult::fromSuccess();
    return TypeReaderResult::fromError(CommandError::ObjectNotFound, "Channel not found.");
}
