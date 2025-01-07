#include "commandinfo.h"
#include "dppcmd/utils/strings.h"

namespace dppcmd
{
    std::span<const std::string> command_info::aliases() const
    {
        std::span<const std::string> namesSpan(names);
        return namesSpan.size() > 1 ? namesSpan.subspan(1) : std::span<const std::string>();
    }

    bool command_info::matches(std::string_view name, bool case_sensitive) const
    {
        return std::ranges::any_of(names, [case_sensitive, name](const std::string& n) {
            return utility::sequals(n, name, case_sensitive);
        });
    }
}
