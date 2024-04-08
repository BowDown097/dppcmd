#include "module.h"
#include <format>

bad_command_argument::bad_command_argument(CommandError error, const std::string& arg, size_t argIndex,
                                           const std::string& module, const std::string& command, const std::string& message)
    : m_error(error), m_message(std::format(
        "{}::{}: Failed to convert argument {} ({}): {}",
        module, command, argIndex, arg, message
      )) {}

std::vector<CommandInfo> ModuleBase::commands() const
{
    std::vector<CommandInfo> out;
    out.reserve(m_commands.size());
    for (const auto& [info, _] : m_commands)
        out.push_back(info);
    return out;
}
