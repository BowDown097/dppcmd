#pragma once
#include <ostream>

namespace dppcmd
{
    enum class command_error
    {
        // search
        unknown_command = 1,
        // parse
        parse_failed,
        bad_arg_count,
        // type reader
        object_not_found,
        multiple_matches,
        // preconditions
        unmet_precondition,
        // execute
        exception,
        // runtime
        unsuccessful
    };
}

inline std::ostream& operator<<(std::ostream& os, const dppcmd::command_error ce)
{
    switch (ce)
    {
    case dppcmd::command_error::unknown_command: return os << "Unknown command";
    case dppcmd::command_error::parse_failed: return os << "Parse failed";
    case dppcmd::command_error::bad_arg_count: return os << "Bad argument count";
    case dppcmd::command_error::object_not_found: return os << "Object not found";
    case dppcmd::command_error::multiple_matches: return os << "Multiple matches";
    case dppcmd::command_error::unmet_precondition: return os << "Precondition unmet";
    case dppcmd::command_error::exception: return os << "Threw exception";
    case dppcmd::command_error::unsuccessful: return os << "Unsuccessful";
    }

    return os;
}
