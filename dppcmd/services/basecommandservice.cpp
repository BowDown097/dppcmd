#include "basecommandservice.h"
#include "dppcmd/commands/commandinfo.h"
#include "dppcmd/commands/commandparser.h"
#include "dppcmd/readers/channeltypereader.h"
#include "dppcmd/readers/guildmembertypereader.h"
#include "dppcmd/readers/roletypereader.h"
#include "dppcmd/readers/usertypereader.h"
#include "dppcmd/utils/join.h"
#include <dpp/dispatcher.h>

namespace dppcmd
{
    base_command_service::base_command_service(dpp::cluster* cluster, command_service_config config)
        : m_cluster(cluster), m_config(config)
    {
        register_type_reader<channel_in>();
        register_type_reader<guild_member_in>();
        register_type_reader<role_in>();
        register_type_reader<user_in>();
    }

    TASK(precondition_result) base_command_service::gen_precondition_result(command_info& command,
                                                                            const dpp::message_create_t* event)
    {
        if (command.preconditions().empty())
            RETURN(precondition_result::from_success());

        std::optional<command_error> finalError;
        std::vector<std::string> messages;

        for (precondition& precond : command.preconditions())
        {
            precondition_result result = AWAIT(precond.check(m_cluster, event, this));
            if (result.success())
                continue;

            command_error error = result.error().value();
            if (finalError)
            {
                if (finalError != error)
                {
                    finalError = command_error::unsuccessful;
                }
            }
            else
            {
                finalError = error;
            }

            messages.push_back(result.message());
        }

        if (finalError)
            RETURN(precondition_result::from_error(finalError.value(), utility::join(messages, '\n')));

        RETURN(precondition_result::from_success());
    }

    TASK(command_result) base_command_service::handle_message(const dpp::message_create_t* event)
    {
        // not really a success, but we don't really want to trigger any error handler so this will suffice.
        if (!event->msg.content.starts_with(m_config.command_prefix))
            RETURN(command_result::from_success());

        std::vector<std::string> args = command_parser::parse(event->msg.content, m_config.separator_char);

        std::string inputCommandName = args.front();
        inputCommandName.erase(inputCommandName.cbegin());
        args.erase(args.cbegin());

        RETURN(AWAIT(run_command(event, inputCommandName, std::move(args))));
    }
}
