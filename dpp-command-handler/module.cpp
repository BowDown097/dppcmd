#include "module.h"
#include <format>

namespace dpp
{
    bad_command_argument::bad_command_argument(command_error error, std::string_view arg, size_t index,
                                               std::string_view module, std::string_view command, std::string_view message)
        : m_error(error), m_message(std::format(
            "{}::{}: Failed to convert argument {} ({}): {}",
            module, command, index, arg, message
          )) {}

    std::vector<std::reference_wrapper<const command_info>> module_base::commands() const
    {
        std::vector<std::reference_wrapper<const command_info>> out;
        out.reserve(m_commands.size());
        for (const auto& [info, _] : m_commands)
            out.push_back(info);
        return out;
    }
}
