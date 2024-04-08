#ifndef PRECONDITIONRESULT_H
#define PRECONDITIONRESULT_H
#include "result.h"

class PreconditionResult : public DppResult
{
public:
    static PreconditionResult fromSuccess(const std::string& message = "")
    { return PreconditionResult(std::nullopt, message); }

    static PreconditionResult fromError(const std::string& message = "")
    { return PreconditionResult(CommandError::UnmetPrecondition, message); }

    static PreconditionResult fromError(const std::exception& e)
    { return PreconditionResult(CommandError::Exception, e.what()); }

    static PreconditionResult fromError(CommandError error, const std::string& message)
    { return PreconditionResult(error, message); }

    PreconditionResult() = default;
private:
    PreconditionResult(const std::optional<CommandError>& error, const std::string& message) : DppResult(error, message) {}
};

#endif // PRECONDITIONRESULT_H
