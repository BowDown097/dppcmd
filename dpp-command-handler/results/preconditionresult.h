#ifndef PRECONDITIONRESULT_H
#define PRECONDITIONRESULT_H
#include "result.h"

class PreconditionResult : public DppResult
{
public:
    static PreconditionResult fromSuccess(std::string_view message = "")
    { return PreconditionResult(std::nullopt, message); }

    static PreconditionResult fromError(std::string_view message = "")
    { return PreconditionResult(CommandError::UnmetPrecondition, message); }

    static PreconditionResult fromError(const std::exception& e)
    { return PreconditionResult(CommandError::Exception, e.what()); }

    static PreconditionResult fromError(CommandError error, std::string_view message)
    { return PreconditionResult(error, message); }

    PreconditionResult() = default;
private:
    PreconditionResult(const std::optional<CommandError>& error, std::string_view message) : DppResult(error, message) {}
};

#endif // PRECONDITIONRESULT_H
