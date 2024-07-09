#include "commandservice.h"
#include "dpp-command-handler/commands/exceptions.h"
#include <format>

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

            if (args.size() >= function->target_arg_count()) // >= to count optional arguments
            {
                try
                {
                    if (function->is_coroutine())
                        RETURN(AWAIT(function->invoke<TASK(command_result)>(std::move(args), m_cluster, event)));
                    else
                        RETURN(function->invoke<command_result>(std::move(args), m_cluster, event));
                }
                catch (const bad_command_argument& e)
                {
                    RETURN(command_result::from_error(e.error(), e.what()));
                }
                catch (const std::exception& e)
                {
                    RETURN(command_result::from_error(e));
                }
            }
            else
            {
                RETURN(command_result::from_error(command_error::bad_arg_count, std::format(
                    "{}{}: Ran with {} arguments, expects at least {}",
                    m_config.command_prefix, name, args.size(), function->target_arg_count()
                )));
            }
        }
        RETURN(command_result::from_success());
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
