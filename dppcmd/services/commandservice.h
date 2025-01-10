#pragma once
#include "basecommandservice.h"
#include "dppcmd/commands/commandexecution.h"
#include "dppcmd/commands/commandinfo.h"
#include "dppcmd/utils/function_traits.h"

namespace dppcmd
{
    class command_service : public base_command_service
    {
    public:
        explicit command_service(dpp::cluster* cluster, command_service_config config = {})
            : base_command_service(cluster, config) {}

        std::vector<const command_info*> commands() const;
        std::vector<const command_info*> search_command(std::string_view name) const override;

        template<typename Function> requires std::is_function_v<std::remove_pointer_t<Function>>
        void register_command(Function fn, command_info info)
        {
            using FTF = utility::function_traits<Function>;
            using Result = FTF::result_type;
            using Args = FTF::args;

            auto cmd_fn = std::make_unique<command_function>();
            cmd_fn->set(command_execution::create_buffer_function<Result, Args>(fn, info.name()));
            cmd_fn->set_target_arg_count(command_execution::target_arg_count<Args>());
            m_commands.emplace_back(info, std::move(cmd_fn));
        }

        template<typename Function> requires std::is_function_v<std::remove_pointer_t<Function>>
        void register_command(Function fn, std::in_place_t, const std::string& name, auto&&... command_info_args)
        {
            command_info info(nullptr, { name }, std::forward<decltype(command_info_args)>(command_info_args)...);
            register_command(std::move(fn), std::move(info));
        }

        template<typename Function> requires std::is_function_v<std::remove_pointer_t<Function>>
        void register_command(Function fn, std::in_place_t,
                              std::initializer_list<std::string> names, auto&&... command_info_args)
        {
            command_info info(nullptr, names, std::forward<decltype(command_info_args)>(command_info_args)...);
            register_command(std::move(fn), std::move(info));
        }
    private:
        std::vector<std::pair<command_info, std::unique_ptr<command_function>>> m_commands;
        TASK(command_result) run_command(const dpp::message_create_t* event, std::string_view name,
                                         std::vector<std::string>&& args) override;
    };
}
