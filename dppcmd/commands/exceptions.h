#pragma once
#include "dppcmd/results/commanderror.h"
#include <exception>

namespace dppcmd
{
    class bad_argument_count : public std::exception
    {
    public:
        bad_argument_count(std::string_view command, size_t arg_count, size_t target_arg_count);
        size_t arg_count() const { return m_arg_count; }
        const std::string& command() const { return m_command; }
        static constexpr command_error error() { return command_error::bad_arg_count; }
        size_t target_arg_count() const { return m_target_arg_count; }
        const char* what() const noexcept override { return m_formatted_message.c_str(); }
    private:
        size_t m_arg_count;
        std::string m_command;
        size_t m_target_arg_count;
        std::string m_formatted_message;
    };

    class bad_command_argument : public std::exception
    {
    public:
        bad_command_argument(command_error error, std::string_view arg, size_t index,
                             std::string_view command, std::string_view message);
        const std::string& arg() const { return m_arg; }
        const std::string& command() const { return m_command; }
        command_error error() const { return m_error; }
        size_t index() const { return m_index; }
        const std::string& message() const { return m_message; }
        const char* what() const noexcept override { return m_formatted_message.c_str(); }
    private:
        std::string m_arg;
        std::string m_command;
        command_error m_error;
        size_t m_index;
        std::string m_message;
        std::string m_formatted_message;
    };
}
