#pragma once
#include "dppcmd/commands/commandexecution.h"
#include "dppcmd/commands/commandinfo.h"
#include "dppcmd/utils/function_traits.h"
#include <any>

namespace dppcmd
{
    class module_service;

    class module_base
    {
        friend class module_service;
    public:
        explicit module_base(std::string_view name, std::string_view summary = "") : m_name(name), m_summary(summary) {}
        virtual ~module_base() = default;

        std::vector<const command_info*> commands() const;
        const std::string& name() const { return m_name; }
        const std::string& summary() const { return m_summary; }

        friend inline std::ostream& operator<<(std::ostream& os, const module_base& m) { return os << m.name(); }
    protected:
        dpp::cluster* cluster{};
        const dpp::message_create_t* context{};
        const module_service* service{};

        template<typename MemberFunction> requires std::is_member_function_pointer_v<MemberFunction>
        void register_command(MemberFunction fn, auto&&... command_info_args)
        {
            command_info info(this, std::forward<decltype(command_info_args)>(command_info_args)...);
            using FTF = utility::function_traits<MemberFunction>;
            using Result = FTF::result_type;
            using Args = FTF::args;
            using Module = FTF::owner_type;

            auto cmd_fn = std::make_unique<command_function>();
            cmd_fn->set(command_execution::create_buffer_function<Result, Args, Module>(std::mem_fn(fn), info.name()));
            cmd_fn->set_target_arg_count(command_execution::target_arg_count<Args>());
            m_commands.emplace_back(info, std::move(cmd_fn));
        }
    private:
        std::vector<std::pair<command_info, std::unique_ptr<command_function>>> m_commands;
        std::string m_name;
        std::string m_summary;

        virtual TASK(command_result) exec(std::string_view command, command_function* function,
            const dpp::message_create_t* context, module_service* service, std::vector<std::string>&& args,
            bool exceptions, const std::any& extra_data = {}) = 0;
    };
}
