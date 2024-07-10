#pragma once
#include "dpp-command-handler/results/commandresult.h"
#include "dpp-command-handler/results/preconditionresult.h"
#include "dpp-command-handler/utils/ezcoro.h"

namespace dpp
{
    class cluster;
    class command_info;
    class message_create_t;

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
        explicit base_command_service(cluster* cluster, command_service_config config = {})
            : m_cluster(cluster), m_config(config) {}

        TASK(precondition_result) gen_precondition_result(command_info& command, const message_create_t* event);
        TASK(command_result) handle_message(const message_create_t* event);

        virtual std::vector<std::reference_wrapper<const command_info>> search_command(std::string_view name) const = 0;
    protected:
        cluster* m_cluster;
        command_service_config m_config;

        virtual TASK(command_result) run_command(const message_create_t* event, std::string_view name,
                                                 std::vector<std::string>&& args) = 0;
    };
}
