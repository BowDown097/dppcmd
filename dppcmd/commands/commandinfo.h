#pragma once
#include "precondition.h"
#include <span>

namespace dppcmd
{
    class module_base;

    class command_info
    {
    public:
        command_info(module_base* module, std::initializer_list<std::string> names, std::string_view summary = "",
                     std::string_view remarks = "", const std::vector<precondition>& preconditions = {})
            : m_module(module), m_names(names), m_preconditions(preconditions), m_remarks(remarks), m_summary(summary) {}

        command_info(module_base* module, const std::string& name, std::string_view summary = "",
                     std::string_view remarks = "", const std::vector<precondition>& preconditions = {})
            : command_info(module, std::initializer_list<std::string> { name }, summary, remarks, preconditions) {}

        std::span<const std::string> aliases() const;
        bool matches(std::string_view name, bool case_sensitive) const;

        const module_base* module() const { return m_module; }
        const std::string& name() const { return m_names.front(); }
        std::span<const std::string> names() const { return m_names; }
        const std::string& remarks() const { return m_remarks; }
        const std::string& summary() const { return m_summary; }

        std::vector<precondition>& preconditions() { return m_preconditions; }
        std::span<const precondition> preconditions() const { return m_preconditions; }

        friend inline bool operator==(const command_info& lhs, const command_info& rhs)
        { return lhs.name() == rhs.name(); }
        friend inline std::ostream& operator<<(std::ostream& os, const command_info& ci)
        { return os << ci.name(); }
    private:
        module_base* m_module;
        std::vector<std::string> m_names;
        std::vector<precondition> m_preconditions;
        std::string m_remarks;
        std::string m_summary;
    };
}

template<>
struct std::hash<dppcmd::command_info>
{
    size_t operator()(const dppcmd::command_info& info) const
    { return std::hash<std::string>()(info.name()); }
};
