#include "roletypereader.h"
#include "dpp-command-handler/utils/lexical_cast.h"
#include "dpp-command-handler/utils/mentions.h"
#include "dpp-command-handler/utils/strings.h"
#include <dpp/cache.h>
#include <dpp/role.h>

TypeReaderResult RoleTypeReader::read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input)
{
    // by mention (1.0)
    if (uint64_t id = dpp::utility::parse_role_mention(input))
        if (dpp::role* role = dpp::find_role(id))
            addResult(role);

    // by ID (0.9)
    if (uint64_t id = dpp::utility::lexical_cast<uint64_t>(input, false))
        if (dpp::role* role = dpp::find_role(id))
            addResult(role);

    // by name (0.7-0.8)
    {
        dpp::cache<dpp::role>* roleCache = dpp::get_role_cache();
        std::shared_lock l(roleCache->get_mutex());

        for (const auto& [_, role] : roleCache->get_container())
            if (dpp::utility::iequals(role->name, input))
                addResult(role, role->name == input ? 0.8f : 0.7f);
    }

    if (hasResult())
        return TypeReaderResult::fromSuccess();
    return TypeReaderResult::fromError(CommandError::ObjectNotFound, "Role not found.");
}
