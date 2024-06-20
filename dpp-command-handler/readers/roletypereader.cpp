#include "roletypereader.h"
#include "dpp-command-handler/extensions/cache.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/dispatcher.h>
#include <dpp/role.h>

namespace dpp
{
    type_reader_result role_in::read(cluster* cluster, const message_create_t* context, std::string_view input)
    {
        add_results_by_mention(context->msg.guild_id, input); // weight: 1.0
        add_results_by_id(context->msg.guild_id, input); // weight: 0.9
        add_results_by_name(context->msg.guild_id, input); // weight: 0.7-0.8

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "Role not found.");
    }

    void role_in::add_results_by_id(const snowflake guild_id, std::string_view input)
    {
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (role* role = find_guild_role(guild_id, id))
                add_result(role);
    }

    void role_in::add_results_by_mention(const snowflake guild_id, std::string_view input)
    {
        if (uint64_t id = utility::parse_role_mention(input))
            if (role* role = find_guild_role(guild_id, id))
                add_result(role);
    }

    void role_in::add_results_by_name(const snowflake guild_id, std::string_view input)
    {
        if (const guild* guild = find_guild(guild_id))
            for (snowflake role_id : guild->roles)
                if (role* role = find_role(role_id); utility::iequals(role->name, input))
                    return add_result(role, role->name == input ? 0.8f : 0.7f);
    }
}
