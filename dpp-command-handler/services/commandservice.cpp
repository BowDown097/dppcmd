#include "commandservice.h"

namespace dpp
{
    TASK(command_result) command_service::run_command(const message_create_t* event, std::string_view name,
                                                      std::vector<std::string>&& args)
    {
        for (auto& [info, function] : m_commands)
        {
            if (!info.matches(name, m_config.case_sensitive_lookup))
                continue;

            precondition_result precond = AWAIT(gen_precondition_result(info, event));
            if (!precond.success())
                RETURN(command_result::from_error(precond.error().value(), precond.message()));

            RETURN(AWAIT(function->invoke_with_result(
                info.name(), args.size(), m_config.throw_exceptions, std::move(args))));
        }

        RETURN(command_result::from_error(command_error::unknown_command, name));
    }

    std::vector<std::reference_wrapper<const command_info>> command_service::search_command(std::string_view name) const
    {
        std::vector<std::reference_wrapper<const command_info>> out;

        for (const auto& [info, _] : m_commands)
            if (info.matches(name, m_config.case_sensitive_lookup))
                out.push_back(info);

        return out;
    }
}
