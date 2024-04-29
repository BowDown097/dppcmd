#include "roletypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/role.h>

namespace dpp
{
    type_reader_result role_in::read(cluster* cluster, const message_create_t* context, std::string_view input)
    {
        if (input.empty())
            return type_reader_result::from_error(command_error::object_not_found, "No input.");

        // by mention (1.0)
        if (uint64_t id = utility::parse_role_mention(input))
            if (role* role = find_role(id))
                add_result(role);

        // by ID (0.9)
        if (uint64_t id = utility::lexical_cast<uint64_t>(input, false))
            if (role* role = find_role(id))
                add_result(role);

        // by name (0.7-0.8)
        {
            cache<role>* roleCache = get_role_cache();
            std::shared_lock l(roleCache->get_mutex());

            for (const auto& [_, role] : roleCache->get_container())
                if (utility::iequals(role->name, input))
                    add_result(role, role->name == input ? 0.8f : 0.7f);
        }

        if (has_result())
            return type_reader_result::from_success();
        return type_reader_result::from_error(command_error::object_not_found, "Role not found.");
    }
}
