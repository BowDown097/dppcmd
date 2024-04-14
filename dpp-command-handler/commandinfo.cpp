#include "commandinfo.h"
#include "utils/strings.h"

std::span<const std::string> CommandInfo::aliases() const
{
    return names().size() > 1 ? names().subspan(1) : std::span<const std::string>();
}

bool CommandInfo::matches(std::string_view name, bool caseSensitive) const
{
    return std::ranges::any_of(m_names, [caseSensitive, name](const std::string& n) {
        return dpp::utility::sequals(n, name, caseSensitive);
    });
}
