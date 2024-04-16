#ifndef MODULESERVICE_H
#define MODULESERVICE_H
#include "results/commandresult.h"
#include "utils/ezcoro.h"
#include <deque>
#include <memory>
#include <span>
#include <vector>

class CommandInfo;
class ModuleBase;
class PreconditionResult;
namespace dpp { class cluster; class message_create_t; }

template<typename T>
concept Module = std::derived_from<T, ModuleBase>;

struct ModuleServiceConfig
{
    bool caseSensitiveLookup{};
    char commandPrefix = '!';
    char separatorChar = ' ';
};

class ModuleService
{
public:
    ModuleService(dpp::cluster* cluster, ModuleServiceConfig config = {})
        : m_cluster(cluster), m_config(config) {}

    TASK(PreconditionResult) genPreconditionResult(const CommandInfo& command, const dpp::message_create_t* event);
    TASK(CommandResult) handleMessage(const dpp::message_create_t* event);

    std::span<const std::unique_ptr<ModuleBase>> modules() const;

    std::vector<std::reference_wrapper<const CommandInfo>> searchCommand(std::string_view name) const;
    std::vector<std::reference_wrapper<const ModuleBase>> searchModule(std::string_view name) const;

    template<Module M>
    void registerModule()
    { m_modules.push_back(std::make_unique<M>()); }

    template<Module... Modules>
    void registerModules()
    { (m_modules.push_back(std::make_unique<Modules>()), ...); }
private:
    dpp::cluster* m_cluster;
    ModuleServiceConfig m_config;
    std::vector<std::unique_ptr<ModuleBase>> m_modules;

    TASK(CommandResult) runCommand(const dpp::message_create_t* event, std::string_view name,
               std::deque<std::string>&& args);
};

#endif // MODULESERVICE_H
