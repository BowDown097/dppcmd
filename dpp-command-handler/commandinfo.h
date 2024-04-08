#ifndef COMMANDINFO_H
#define COMMANDINFO_H
#include "precondition.h"

class ModuleBase;

class CommandInfo
{
public:
    CommandInfo(ModuleBase* module, const std::string& name, const std::string& summary = "",
                const std::string& remarks = "", const std::vector<Precondition> preconditions = {})
        : m_module(module), m_name(name), m_preconditions(preconditions), m_summary(summary), m_remarks(remarks) {}

    const ModuleBase* module() const { return m_module; }
    std::string name() const { return m_name; }
    std::vector<Precondition> preconditions() const { return m_preconditions; }
    std::string remarks() const { return m_remarks; }
    std::string summary() const { return m_summary; }

    friend inline bool operator==(const CommandInfo& lhs, const CommandInfo& rhs) { return lhs.m_name == rhs.m_name; }
    friend inline std::ostream& operator<<(std::ostream& os, const CommandInfo& ci) { return os << ci.m_name; }
private:
    ModuleBase* m_module;
    std::string m_name;
    std::vector<Precondition> m_preconditions;
    std::string m_remarks;
    std::string m_summary;
};

struct CommandInfoHash
{
    size_t operator()(const CommandInfo& info) const
    { return std::hash<std::string>{}(info.name()); }
};

#endif // COMMANDINFO_H
