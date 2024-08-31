#pragma once
#include "dppcmd/readers/typereader.h"
#include "dppcmd/results/commandresult.h"
#include "dppcmd/results/preconditionresult.h"
#include "dppcmd/utils/ezcoro.h"
#include <typeindex>

namespace dpp { class cluster; class message_create_t; }

namespace dppcmd
{
    class command_info;

    struct command_service_config
    {
        bool case_sensitive_lookup{};
        char command_prefix = '!';
        char separator_char = ' ';
        bool throw_exceptions{};
    };

    class base_command_service
    {
    public:
        explicit base_command_service(dpp::cluster* cluster, command_service_config config = {});
        TASK(precondition_result) gen_precondition_result(command_info& command, const dpp::message_create_t* event);
        TASK(command_result) handle_message(const dpp::message_create_t* event);

        dpp::cluster* cluster() const { return m_cluster; }
        command_service_config config() const { return m_config; }

        template<typename T>
        std::unique_ptr<type_reader<T>> create_type_reader() const
        {
            using Decayed = std::decay_t<T>;
            if (auto it = m_type_reader_factories.find(typeid(Decayed)); it != m_type_reader_factories.end())
                return std::unique_ptr<type_reader<Decayed>>(static_cast<type_reader<Decayed>*>(it->second()));
            return nullptr;
        }

        template<typename T> requires utility::is_type_reader<T>
        void register_type_reader()
        {
            // this allocation is safe because get_type_reader() ensures proper management
            m_type_reader_factories[typeid(typename T::value_type)] = []() -> void* { return new T; };
        }

        virtual std::vector<const command_info*> search_command(std::string_view name) const = 0;
    protected:
        dpp::cluster* m_cluster;
        command_service_config m_config;
        std::unordered_map<std::type_index, std::function<void*()>> m_type_reader_factories;

        virtual TASK(command_result) run_command(const dpp::message_create_t* event, std::string_view name,
                                                 std::vector<std::string>&& args) = 0;
    };
}
