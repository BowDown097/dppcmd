#ifndef RESULT_H
#define RESULT_H
#include "commanderror.h"
#include <optional>
#include <string>

class DppResult
{
public:
    std::optional<CommandError> error() const { return m_error; }
    std::string message() const { return m_message; }
    bool success() const { return !m_error; }
    DppResult() : m_error(std::nullopt) {}
protected:
    std::optional<CommandError> m_error;
    std::string m_message;
    DppResult(const std::optional<CommandError>& error, const std::string& message)
        : m_error(error), m_message(message) {}
};

#endif // RESULT_H
