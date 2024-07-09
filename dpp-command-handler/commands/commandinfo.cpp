#include "commandinfo.h"
#include "dpp-command-handler/utils/strings.h"

namespace dpp
{
    std::span<const std::string> command_info::aliases() const
    {
        return names().size() > 1 ? names().subspan(1) : std::span<const std::string>();
    }

    bool command_info::matches(std::string_view name, bool case_sensitive) const
    {
        return std::ranges::any_of(m_names, [case_sensitive, name](const std::string& n) {
            return utility::sequals(n, name, case_sensitive);
        });
    }
}
