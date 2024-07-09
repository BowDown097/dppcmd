#pragma once
#include "basecommandservice.h"
#include <any>
#include <span>

namespace dpp
{
    class module_base;

    template<typename T>
    concept module_derivative = std::derived_from<T, module_base>;

    class module_service : public base_command_service
    {
    public:
        explicit module_service(cluster* cluster, command_service_config config = {})
            : base_command_service(cluster, config) {}

        std::span<const std::unique_ptr<module_base>> modules() const { return m_modules; }

        std::vector<std::reference_wrapper<const command_info>> search_command(std::string_view name) const override;
        std::vector<std::reference_wrapper<const module_base>> search_module(std::string_view name) const;

        template<module_derivative M>
        void register_module()
        {
            m_modules.push_back(std::make_unique<M>());
        }

        template<module_derivative M>
        void register_module(auto&& extra_data)
        {
            register_module<M>();
            m_extra_module_data.emplace(m_modules.back().get(), std::forward<decltype(extra_data)>(extra_data));
        }

        template<module_derivative... Modules>
        void register_modules()
        {
            (register_module<Modules>(), ...);
        }
    private:
        std::unordered_map<module_base*, std::any> m_extra_module_data;
        std::vector<std::unique_ptr<module_base>> m_modules;

        TASK(command_result) run_command(const message_create_t* event, std::string_view name,
                                         std::vector<std::string>&& args) override;
    };
}
