#pragma once
#include "precondition.h"
#include <span>

namespace dppcmd
{
    class module_base;

    struct command_info
    {
        const module_base* module{}; // do not worry about initializing this yourself! the module service will do it.
        std::vector<std::string> names;
        std::string summary;
        std::string remarks;
        std::vector<precondition> preconditions;

        std::span<const std::string> aliases() const;
        bool matches(std::string_view name, bool case_sensitive) const;

        const std::string& name() const { return names.front(); }

        friend inline bool operator==(const command_info& lhs, const command_info& rhs)
        { return lhs.name() == rhs.name(); }
        friend inline std::ostream& operator<<(std::ostream& os, const command_info& ci)
        { return os << ci.name(); }
    };
}

template<>
struct std::hash<dppcmd::command_info>
{
    size_t operator()(const dppcmd::command_info& info) const
    { return std::hash<std::string>()(info.name()); }
};
