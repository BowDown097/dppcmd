#pragma once
#include "basecommandservice.h"
#include <any>
#include <span>

namespace dppcmd
{
    class module_base;

    class module_service : public base_command_service
    {
    public:
        explicit module_service(dpp::cluster* cluster, command_service_config config = {})
            : base_command_service(cluster, config) {}

        std::span<const std::unique_ptr<module_base>> modules() const { return m_modules; }

        std::vector<const command_info*> search_command(std::string_view name) const override;
        std::vector<const module_base*> search_module(std::string_view name) const;

        template<std::derived_from<module_base> M>
        void register_module()
        {
            m_modules.push_back(std::make_unique<M>());
        }

        template<std::derived_from<module_base> M>
        void register_module(auto&& extra_data)
        {
            register_module<M>();
            m_extra_module_data.emplace(m_modules.back().get(), std::forward<decltype(extra_data)>(extra_data));
        }

        template<std::derived_from<module_base>... Modules>
        void register_modules()
        {
            (register_module<Modules>(), ...);
        }
    private:
        std::unordered_map<module_base*, std::any> m_extra_module_data;
        std::vector<std::unique_ptr<module_base>> m_modules;

        TASK(command_result) run_command(const dpp::message_create_t* event, std::string_view name,
                                         std::vector<std::string>&& args) override;
    };
}
