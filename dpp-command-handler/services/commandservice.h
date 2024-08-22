#pragma once
#include "basecommandservice.h"
#include "dpp-command-handler/commands/commandexecution.h"
#include "dpp-command-handler/commands/commandinfo.h"
#include "dpp-command-handler/utils/function_traits.h"

namespace dpp
{
    class command_service : public base_command_service
    {
    public:
        explicit command_service(dpp::cluster* cluster, command_service_config config = {})
            : base_command_service(cluster, config) {}

        std::vector<std::reference_wrapper<const command_info>> search_command(std::string_view name) const override;

        template<typename F>
        void register_command(F&& fn, auto&&... command_info_args)
        {
            command_info info(nullptr, std::forward<decltype(command_info_args)>(command_info_args)...);
            using FTF = utility::function_traits<F>;
            using Result = FTF::result_type;
            using Args = FTF::args;

            auto cmd_fn = std::make_unique<command_function>();
            cmd_fn->set(command_execution::create_buffer_function<Result, Args>(std::forward<F>(fn), info.name()));
            cmd_fn->set_target_arg_count(command_execution::target_arg_count<Args>());
            m_commands.emplace_back(info, std::move(cmd_fn));
        }
    private:
        std::vector<std::pair<command_info, std::unique_ptr<command_function>>> m_commands;
        TASK(command_result) run_command(const message_create_t* event, std::string_view name,
                                         std::vector<std::string>&& args) override;
    };
}
