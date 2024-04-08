#ifndef COMMANDRESULT_H
#define COMMANDRESULT_H
#include "result.h"

class CommandResult : public DppResult
{
public:
    static CommandResult fromSuccess(const std::string& message = "")
    { return CommandResult(std::nullopt, message); }

    static CommandResult fromError(const std::string& message = "")
    { return CommandResult(CommandError::Unsuccessful, message); }

    static CommandResult fromError(const std::exception& e)
    { return CommandResult(CommandError::Exception, e.what()); }

    static CommandResult fromError(CommandError error, const std::string& message)
    { return CommandResult(error, message); }

    CommandResult() = default;
private:
    CommandResult(const std::optional<CommandError>& error, const std::string& message) : DppResult(error, message) {}
};

#endif // COMMANDRESULT_H
