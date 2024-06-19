#include "moduleservice.h"
#include "commandparser.h"
#include "module.h"
#include "utils/strings.h"
#include <dpp/dispatcher.h>
#include <format>

namespace dpp
{
    TASK(precondition_result) module_service::gen_precondition_result(const command_info& command,
                                                                      const message_create_t* event)
    {
        if (command.preconditions().empty())
            RETURN(precondition_result::from_success());

        std::optional<command_error> finalError;
        std::vector<std::string> messages;

        for (precondition& precond : command.preconditions())
        {
            precondition_result result = AWAIT(precond.check(m_cluster, event, this));
            if (result.success())
                continue;

            command_error error = result.error().value();
            if (finalError)
            {
                if (finalError != error)
                {
                    finalError = command_error::unsuccessful;
                }
            }
            else
            {
                finalError = error;
            }

            messages.push_back(result.message());
        }

        if (finalError)
            RETURN(precondition_result::from_error(finalError.value(), utility::join(messages, '\n')));

        RETURN(precondition_result::from_success());
    }

    TASK(command_result) module_service::handle_message(const message_create_t* event)
    {
        // not really a success, but we don't really want to trigger any error handler so this will suffice.
        if (!event->msg.content.starts_with(m_config.command_prefix))
            RETURN(command_result::from_success());

        std::vector<std::string> args = command_parser::parse(event->msg.content, m_config.separator_char);

        std::string inputCommandName = args.front();
        inputCommandName.erase(inputCommandName.cbegin());
        args.erase(args.cbegin());

        RETURN(AWAIT(run_command(event, inputCommandName, std::move(args))));
    }

    std::span<const std::unique_ptr<module_base>> module_service::modules() const
    {
        return m_modules;
    }

    TASK(command_result) module_service::run_command(const message_create_t* event, std::string_view name,
                                                     std::vector<std::string>&& args)
    {
        for (std::unique_ptr<module_base>& module : m_modules)
        {
            for (const auto& [info, function] : module->m_commands)
            {
                if (!info.matches(name, m_config.case_sensitive_lookup))
                    continue;

                precondition_result precond = AWAIT(gen_precondition_result(info, event));
                if (!precond.success())
                    RETURN(command_result::from_error(precond.error().value(), precond.message()));

                if (args.size() >= function->target_arg_count()) // >= to count optional arguments
                {
                    if (auto it = m_extra_module_data.find(module.get()); it != m_extra_module_data.end())
                    {
                        RETURN(AWAIT(module->execute_command(function.get(), m_cluster, event,
                                                             this, std::move(args), it->second)));
                    }
                    else
                    {
                        RETURN(AWAIT(module->execute_command(function.get(), m_cluster, event, this, std::move(args))));
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
        }

        RETURN(command_result::from_error(command_error::unknown_command, name));
    }

    std::vector<std::reference_wrapper<const command_info>> module_service::search_command(std::string_view name) const
    {
        std::vector<std::reference_wrapper<const command_info>> out;

        for (const std::unique_ptr<module_base>& module : m_modules)
            for (const auto& [info, _] : module->m_commands)
                if (info.matches(name, m_config.case_sensitive_lookup))
                    out.push_back(std::cref(info));

        return out;
    }

    std::vector<std::reference_wrapper<const module_base>> module_service::search_module(std::string_view name) const
    {
        std::vector<std::reference_wrapper<const module_base>> out;

        for (const std::unique_ptr<module_base>& module : m_modules)
            if (utility::sequals(module->name(), name, m_config.case_sensitive_lookup))
                out.push_back(std::cref(*module));

        return out;
    }
}
