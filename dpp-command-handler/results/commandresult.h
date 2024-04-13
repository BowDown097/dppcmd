#ifndef COMMANDRESULT_H
#define COMMANDRESULT_H
#include "result.h"

class CommandResult : public DppResult
{
public:
    static CommandResult fromSuccess(std::string_view message = "")
    { return CommandResult(std::nullopt, message); }

    static CommandResult fromError(std::string_view message = "")
    { return CommandResult(CommandError::Unsuccessful, message); }

    static CommandResult fromError(const std::exception& e)
    { return CommandResult(CommandError::Exception, e.what()); }

    static CommandResult fromError(CommandError error, std::string_view message)
    { return CommandResult(error, message); }

    CommandResult() = default;
private:
    CommandResult(const std::optional<CommandError>& error, std::string_view message) : DppResult(error, message) {}
};

#endif // COMMANDRESULT_H
