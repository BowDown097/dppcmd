#include "modulebase.h"

namespace dppcmd
{
    std::vector<const command_info*> module_base::commands() const
    {
        std::vector<const command_info*> out;
        out.reserve(m_commands.size());
        for (const auto& [info, _] : m_commands)
            out.push_back(&info);
        return out;
    }
}
