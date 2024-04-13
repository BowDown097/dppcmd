#ifndef TYPEREADERRESULT_H
#define TYPEREADERRESULT_H
#include "result.h"

class TypeReaderResult : public DppResult
{
public:
    static TypeReaderResult fromSuccess(std::string_view message = "")
    { return TypeReaderResult(std::nullopt, message); }

    static TypeReaderResult fromError(CommandError error, std::string_view message)
    { return TypeReaderResult(error, message); }

    TypeReaderResult() = default;
private:
    TypeReaderResult(const std::optional<CommandError>& error, std::string_view message) : DppResult(error, message) {}
};

#endif // TYPEREADERRESULT_H
