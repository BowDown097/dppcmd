#ifndef MODULESERVICE_H
#define MODULESERVICE_H
#include "results/commandresult.h"
#include "utils/ezcoro.h"
#include <deque>
#include <memory>
#include <span>
#include <vector>

namespace dpp
{
    class cluster;
    class command_info;
    class message_create_t;
    class module_base;
    class precondition_result;

    template<typename T>
    concept module_derivative = std::derived_from<T, module_base>;

    struct module_service_config
    {
        bool case_sensitive_lookup{};
        char command_prefix = '!';
        char separator_char = ' ';
    };

    class module_service
    {
    public:
        module_service(cluster* cluster, module_service_config config = {})
            : m_cluster(cluster), m_config(config) {}

        TASK(precondition_result) gen_precondition_result(const command_info& command, const message_create_t* event);
        TASK(command_result) handle_message(const message_create_t* event);

        std::span<const std::unique_ptr<module_base>> modules() const;

        std::vector<std::reference_wrapper<const command_info>> search_command(std::string_view name) const;
        std::vector<std::reference_wrapper<const module_base>> search_module(std::string_view name) const;

        template<module_derivative M>
        void register_module()
        { m_modules.push_back(std::make_unique<M>()); }

        template<module_derivative... Modules>
        void register_modules()
        { (m_modules.push_back(std::make_unique<Modules>()), ...); }
    private:
        cluster* m_cluster;
        module_service_config m_config;
        std::vector<std::unique_ptr<module_base>> m_modules;

        TASK(command_result) run_command(const message_create_t* event, std::string_view name,
                                         std::deque<std::string>&& args);
    };
}

#endif // MODULESERVICE_H
