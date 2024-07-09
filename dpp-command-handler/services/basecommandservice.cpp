#include "basecommandservice.h"
#include "dpp-command-handler/commands/commandinfo.h"
#include "dpp-command-handler/commands/commandparser.h"
#include "dpp-command-handler/utils/join.h"
#include <dpp/dispatcher.h>

namespace dpp
{
    TASK(precondition_result) base_command_service::gen_precondition_result(command_info& command,
                                                                            const message_create_t* event)
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

    TASK(command_result) base_command_service::handle_message(const message_create_t* event)
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
