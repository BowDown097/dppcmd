#include "module.h"
#include <format>

bad_command_argument::bad_command_argument(CommandError error, std::string_view arg, size_t argIndex,
                                           std::string_view module, std::string_view command, std::string_view message)
    : m_error(error), m_message(std::format(
        "{}::{}: Failed to convert argument {} ({}): {}",
        module, command, argIndex, arg, message
      )) {}

std::vector<CommandCRef> ModuleBase::commands() const
{
    std::vector<CommandCRef> out;
    out.reserve(m_commands.size());
    for (const auto& [info, _] : m_commands)
        out.push_back(std::cref(info));
    return out;
}
