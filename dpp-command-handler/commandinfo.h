#ifndef COMMANDINFO_H
#define COMMANDINFO_H
#include "precondition.h"
#include <span>

class ModuleBase;

class CommandInfo
{
public:
    CommandInfo(ModuleBase* module, std::initializer_list<std::string> names, std::string_view summary = "",
                std::string_view remarks = "", const std::vector<Precondition>& preconditions = {})
        : m_module(module), m_names(names), m_preconditions(preconditions), m_remarks(remarks), m_summary(summary) {}

    CommandInfo(ModuleBase* module, const std::string& name, std::string_view summary = "",
                std::string_view remarks = "", const std::vector<Precondition>& preconditions = {})
        : CommandInfo(module, std::initializer_list<std::string> { name }, summary, remarks, preconditions) {}

    std::span<const std::string> aliases() const;
    bool matches(std::string_view name, bool caseSensitive) const;

    const ModuleBase* module() const { return m_module; }
    std::string name() const { return m_names.front(); }
    std::span<const std::string> names() const { return m_names; }
    std::vector<Precondition> preconditions() const { return m_preconditions; }
    std::string remarks() const { return m_remarks; }
    std::string summary() const { return m_summary; }

    friend inline bool operator==(const CommandInfo& lhs, const CommandInfo& rhs)
    { return lhs.name() == rhs.name(); }
    friend inline std::ostream& operator<<(std::ostream& os, const CommandInfo& ci)
    { return os << ci.name(); }
private:
    ModuleBase* m_module;
    std::vector<std::string> m_names;
    std::vector<Precondition> m_preconditions;
    std::string m_remarks;
    std::string m_summary;
};

template<>
struct std::hash<CommandInfo>
{
    size_t operator()(const CommandInfo& info) const
    { return std::hash<std::string>()(info.name()); }
};

#endif // COMMANDINFO_H
