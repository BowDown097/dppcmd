#pragma once
#include "result.h"

namespace dpp
{
    class precondition_result : public result
    {
    public:
        static precondition_result from_success(std::string_view message = "")
        { return precondition_result(std::nullopt, message); }

        static precondition_result from_error(std::string_view message = "")
        { return precondition_result(command_error::unmet_precondition, message); }

        static precondition_result from_error(const std::exception& e)
        { return precondition_result(command_error::exception, e.what()); }

        static precondition_result from_error(command_error error, std::string_view message)
        { return precondition_result(error, message); }

        precondition_result() = default;
    private:
        precondition_result(const std::optional<command_error>& error, std::string_view message) : result(error, message) {}
    };
}
