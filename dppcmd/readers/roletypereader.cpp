#include "roletypereader.h"
#include "dppcmd/extensions/cache.h"
#include "dppcmd/utils/lexical_cast.h"
#include "dppcmd/utils/mentions.h"
#include "dppcmd/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/dispatcher.h>
#include <dpp/role.h>

namespace dppcmd
{
    type_reader_result role_in::read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input)
    {
        add_results_by_mention(context->msg.guild_id, input); // weight: 1.0
        add_results_by_id(context->msg.guild_id, input); // weight: 0.9
        add_results_by_name(context->msg.guild_id, input); // weight: 0.7-0.8

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "Role not found.");
    }

    void role_in::add_results_by_id(const dpp::snowflake guild_id, std::string_view input)
    {
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (dpp::role* role = find_guild_role(guild_id, id))
                if (id_set.insert(id).second)
                    add_result(role);
    }

    void role_in::add_results_by_mention(const dpp::snowflake guild_id, std::string_view input)
    {
        if (uint64_t id = utility::parse_role_mention(input))
            if (dpp::role* role = find_guild_role(guild_id, id))
                if (id_set.insert(id).second)
                    add_result(role);
    }

    void role_in::add_results_by_name(const dpp::snowflake guild_id, std::string_view input)
    {
        if (const dpp::guild* guild = find_guild(guild_id))
            for (dpp::snowflake role_id : guild->roles)
                if (dpp::role* role = find_role(role_id); utility::iequals(role->name, input))
                    if (id_set.insert(role_id).second)
                        return add_result(role, role->name == input ? 0.8f : 0.7f);
    }
}
