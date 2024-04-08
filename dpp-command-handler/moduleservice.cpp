#include "moduleservice.h"
#include "commandparser.h"
#include "module.h"
#include "utils/join.h"
#include "utils/strings.h"
#include <dpp/dispatcher.h>
#include <format>

TASK(PreconditionResult) ModuleService::genPreconditionResult(const CommandInfo& command,
                                                              const dpp::message_create_t* event)
{
    if (command.preconditions().empty())
        RETURN(PreconditionResult::fromSuccess());

    std::optional<CommandError> finalError;
    std::vector<std::string> messages;

    for (Precondition& precond : command.preconditions())
    {
        PreconditionResult result = AWAIT(precond.check(m_cluster, event, this));
        if (result.success())
            continue;

        CommandError error = result.error().value();
        if (finalError)
        {
            if (finalError != error)
            {
                finalError = CommandError::Unsuccessful;
            }
        }
        else
        {
            finalError = error;
        }

        messages.push_back(result.message());
    }

    if (finalError)
        RETURN(PreconditionResult::fromError(finalError.value(), cmdhndlrutils::join(messages, "\n")));

    RETURN(PreconditionResult::fromSuccess());
}

TASK(CommandResult) ModuleService::handleMessage(const dpp::message_create_t* event)
{
    // not really a success, but we don't really want to trigger any error handler so this will suffice.
    if (!event->msg.content.starts_with(m_config.commandPrefix))
        RETURN(CommandResult::fromSuccess());

    std::deque<std::string> args = CommandParser::parseArguments(event->msg.content, m_config.separatorChar);

    std::string inputCommandName = args.front();
    inputCommandName.erase(inputCommandName.cbegin());
    args.pop_front();

    RETURN(AWAIT(runCommand(event, inputCommandName, std::move(args))));
}

std::span<const std::unique_ptr<ModuleBase>> ModuleService::modules() const
{
    return m_modules;
}

TASK(CommandResult) ModuleService::runCommand(const dpp::message_create_t* event, const std::string& name,
                                              std::deque<std::string>&& args)
{
    for (std::unique_ptr<ModuleBase>& module : m_modules)
    {
        for (const auto& [info, function] : module->m_commands)
        {
            if (!cmdhndlrutils::sequals(info.name(), name, m_config.caseSensitiveLookup))
                continue;

            PreconditionResult precond = AWAIT(genPreconditionResult(info, event));
            if (!precond.success())
                RETURN(CommandResult::fromError(precond.error().value(), precond.message()));

            if (args.size() >= function->targetArgCount()) // >= to count optional arguments
            {
                RETURN(AWAIT(module->createInstanceAndRun(function.get(), m_cluster, event, this, std::move(args))));
            }
            else
            {
                RETURN(CommandResult::fromError(CommandError::BadArgCount, std::format(
                    "{}::{}: Ran with {} arguments, expects at least {}",
                    module->name(), info.name(), args.size(), function->targetArgCount()
                )));
            }
        }
    }

    RETURN(CommandResult::fromError(CommandError::UnknownCommand, name));
}

std::vector<CommandCRef> ModuleService::searchCommand(const std::string& name, bool caseSensitive) const
{
    std::vector<CommandCRef> out;

    for (const std::unique_ptr<ModuleBase>& module : m_modules)
        for (const auto& [info, _] : module->m_commands)
            if (cmdhndlrutils::sequals(info.name(), name, m_config.caseSensitiveLookup))
                out.push_back(std::cref(info));

    return out;
}

std::vector<ModuleCRef> ModuleService::searchModule(const std::string& name, bool caseSensitive) const
{
    std::vector<ModuleCRef> out;

    for (const std::unique_ptr<ModuleBase>& module : m_modules)
        if (cmdhndlrutils::sequals(module->name(), name, m_config.caseSensitiveLookup))
            out.push_back(std::cref(*module));

    return out;
}
