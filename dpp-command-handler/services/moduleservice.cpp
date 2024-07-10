#include "moduleservice.h"
#include "dpp-command-handler/modules/modulebase.h"
#include "dpp-command-handler/utils/strings.h"

namespace dpp
{
    TASK(command_result) module_service::run_command(const message_create_t* event, std::string_view name,
                                                     std::vector<std::string>&& args)
    {
        for (std::unique_ptr<module_base>& module : m_modules)
        {
            for (auto& [info, function] : module->m_commands)
            {
                if (!info.matches(name, m_config.case_sensitive_lookup))
                    continue;

                precondition_result precond = AWAIT(gen_precondition_result(info, event));
                if (!precond.success())
                    RETURN(command_result::from_error(precond.error().value(), precond.message()));

                if (auto it = m_extra_module_data.find(module.get()); it != m_extra_module_data.end())
                {
                    RETURN(AWAIT(module->exec(info.name(), function.get(), m_cluster, event, this,
                                              std::move(args), m_config.throw_exceptions, it->second)));
                }
                else
                {
                    RETURN(AWAIT(module->exec(info.name(), function.get(), m_cluster, event, this,
                                              std::move(args), m_config.throw_exceptions)));
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
                    out.push_back(info);

        return out;
    }

    std::vector<std::reference_wrapper<const module_base>> module_service::search_module(std::string_view name) const
    {
        std::vector<std::reference_wrapper<const module_base>> out;

        for (const std::unique_ptr<module_base>& module : m_modules)
            if (utility::sequals(module->name(), name, m_config.case_sensitive_lookup))
                out.push_back(*module);

        return out;
    }
}
