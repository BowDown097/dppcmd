#ifndef COMMANDERROR_H
#define COMMANDERROR_H
#include <ostream>

enum class CommandError
{
    // search
    UnknownCommand = 1,
    // parse
    ParseFailed,
    BadArgCount,
    // type reader
    ObjectNotFound,
    MultipleMatches,
    // preconditions
    UnmetPrecondition,
    // execute
    Exception,
    // runtime
    Unsuccessful
};

inline std::ostream& operator<<(std::ostream& os, const CommandError ce)
{
    switch (ce)
    {
    case CommandError::UnknownCommand: return os << "Unknown command";
    case CommandError::ParseFailed: return os << "Parse failed";
    case CommandError::BadArgCount: return os << "Bad argument count";
    case CommandError::ObjectNotFound: return os << "Object not found";
    case CommandError::MultipleMatches: return os << "Multiple matches";
    case CommandError::UnmetPrecondition: return os << "Precondition unmet";
    case CommandError::Exception: return os << "Threw exception";
    case CommandError::Unsuccessful: return os << "Unsuccessful";
    }

    return os;
}

#endif // COMMANDERROR_H
