#include "exceptions.h"
#include <format>

namespace dpp
{
    bad_command_argument::bad_command_argument(command_error error, std::string_view arg, size_t index,
                                               std::string_view command, std::string_view message)
        : m_error(error), m_message(std::format(
              "{}: Failed to convert argument {} ({}): {}",
              command, index, arg, message
          )) {}
}
