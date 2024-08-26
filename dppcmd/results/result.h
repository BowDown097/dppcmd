#pragma once
#include "commanderror.h"
#include <optional>
#include <string>

namespace dppcmd
{
    class result
    {
    public:
        std::optional<command_error> error() const { return m_error; }
        const std::string& message() const { return m_message; }
        bool success() const { return !m_error; }
        result() : m_error(std::nullopt) {}
    protected:
        std::optional<command_error> m_error;
        std::string m_message;
        result(const std::optional<command_error>& error, std::string_view message)
            : m_error(error), m_message(message) {}
    };
}
