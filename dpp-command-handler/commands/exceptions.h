#pragma once
#include "dpp-command-handler/results/commanderror.h"
#include <exception>

namespace dpp
{
    class bad_command_argument : public std::exception
    {
    public:
        bad_command_argument(command_error error, std::string_view arg, size_t index,
                             std::string_view command, std::string_view message);
        command_error error() const { return m_error; }
        const char* what() const noexcept override { return m_message.c_str(); }
    private:
        command_error m_error;
        std::string m_message;
    };
}
