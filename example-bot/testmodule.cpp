#include "testmodule.h"
#include "dppcmd/services/moduleservice.h"
#include <dpp/cluster.h>
#include <format>

TestModule::TestModule() : dppcmd::module<TestModule>("Test Module", "Test module.")
{
    register_command(&TestModule::commands, dppcmd::command_info {
        .names = { "testmodule-commands", "testmodule-cmds" },
        .summary = "See the commands in the test module",
        .remarks = "waow"
    });
    register_command(&TestModule::testInput, std::in_place,
        "testinput", "Test some input", "^testinput [str] [num] [dnum] <member>");
}

dppcmd::command_result TestModule::commands(const std::optional<std::string>& filter)
{
    std::vector<const dppcmd::command_info*> commands;
    if (filter.has_value())
        commands = service->search_command(filter.value());
    else
        commands = dppcmd::module_base::commands();

    if (commands.empty())
        return dppcmd::command_result::from_error("No commands were found.");

    return dppcmd::command_result::from_success(dppcmd::utility::join(commands, '\n', [](const dppcmd::command_info* cmd) {
        std::string out;
        if (!cmd->name().empty())
            out += "Name: " + cmd->name() + '\n';
        if (!cmd->summary.empty())
            out += "Summary: " + cmd->summary + '\n';
        if (!cmd->remarks.empty())
            out += "Remarks: " + cmd->remarks + '\n';
        if (!cmd->aliases().empty())
            out += "Aliases: " + dppcmd::utility::join(cmd->aliases(), ", ") + '\n';
        return out;
    }));
}

dppcmd::command_result TestModule::testInput(const std::string& str, int num, double dnum,
    const std::optional<dppcmd::remainder<dpp::guild_member>> memberOpt)
{
    if (memberOpt.has_value())
    {
        return dppcmd::command_result::from_success(std::format("Testing input. {} {} {} {} {} {}",
            str, num, dnum, cluster->rest_ping, memberOpt.value()->get_nickname(), (uint64_t)memberOpt.value()->user_id));
    }
    else
    {
        return dppcmd::command_result::from_success(std::format("Testing input. {} {} {} {}",
            str, num, dnum, cluster->rest_ping));
    }
}
