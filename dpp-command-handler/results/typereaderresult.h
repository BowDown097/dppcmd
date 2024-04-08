#ifndef TYPEREADERRESULT_H
#define TYPEREADERRESULT_H
#include "result.h"

class TypeReaderResult : public DppResult
{
public:
    static TypeReaderResult fromSuccess(const std::string& message = "")
    { return TypeReaderResult(std::nullopt, message); }

    static TypeReaderResult fromError(CommandError error, const std::string& message)
    { return TypeReaderResult(error, message); }

    TypeReaderResult() = default;
private:
    TypeReaderResult(const std::optional<CommandError>& error, const std::string& message) : DppResult(error, message) {}
};

#endif // TYPEREADERRESULT_H
