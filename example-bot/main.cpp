#include "credentials.h"
#include "epicenum.h"
#include "dppcmd/services/commandservice.h"
#include "dppcmd/services/moduleservice.h"
#include "testmodule.h"
#include <dpp/cluster.h>

dpp::task<void> on_message_create(dppcmd::command_service* commands, dppcmd::module_service* modules,
                                  const dpp::message_create_t& event)
{
    std::string_view content = event.msg.content;
    if (!content.starts_with(commands->config().command_prefix))
        co_return;

    std::vector<const dppcmd::command_info*> cs_cmds = commands->search_command(
        content.substr(1, content.find(commands->config().separator_char) - 1));
    std::vector<const dppcmd::command_info*> ms_cmds = modules->search_command(
        content.substr(1, content.find(modules->config().separator_char) - 1));

    try
    {
        dppcmd::command_result result;
        if (!cs_cmds.empty())
            result = co_await commands->handle_message(&event);
        else if (!ms_cmds.empty())
            result = co_await modules->handle_message(&event);

        if (!result.success() || !result.message().empty())
            event.reply(result.message());
    }
    catch (const std::exception& e)
    {
        event.reply(std::format("Exception: {}", e.what()));
    }
}

dppcmd::command_result abcd(const dpp::message_create_t* context, const dppcmd::remainder<std::string>& input)
{
    return dppcmd::command_result::from_success(std::format("Message sent from {}: {}",
        context->msg.author.get_mention(), input.value()));
}

dppcmd::command_result commandServiceCommands(const dppcmd::command_service* svc, const std::optional<std::string>& filter)
{
    std::vector<const dppcmd::command_info*> commands;
    if (filter.has_value())
        commands = svc->search_command(filter.value());
    else
        commands = svc->commands();

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

dppcmd::command_result epicCommand(EpicEnum epicEnum)
{
    return dppcmd::command_result::from_error(std::format("Degree of epicness: {}", static_cast<int>(epicEnum)));
}

dpp::task<dppcmd::command_result> testTaskCommand(dpp::cluster* cluster, const dpp::message_create_t* context)
{
    dpp::channel channel;
    channel.guild_id = context->msg.guild_id;
    channel.name = "channel-from-silly-bot";
    channel.topic = "This channel was created by a very silly bot.";

    dpp::confirmation_callback_t conf = co_await cluster->co_channel_create(channel);
    if (conf.is_error())
        co_return dppcmd::command_result::from_error("Failed to create channel :( " + conf.get_error().human_readable);

    co_return dppcmd::command_result::from_success("Created channel woah " + conf.get<dpp::channel>().get_mention());
}

int main()
{
    Credentials::instance().initialize();

    auto cluster = std::make_unique<dpp::cluster>(
        Credentials::instance().token(),
        dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_members
    );

    auto commands = std::make_unique<dppcmd::command_service>(cluster.get(), dppcmd::command_service_config {
        .command_prefix = '^',
        .throw_exceptions = true
    });

    commands->register_type_reader<EpicEnumReader>();

    commands->register_command(abcd, std::in_place,
        "abcd", "blablabla", "^abcd [input]");
    commands->register_command(commandServiceCommands, std::in_place,
        { "commandservice-commands", "commandservice-cmds" }, "See the commands in the command service");
    commands->register_command(epicCommand, dppcmd::command_info {
        .names = { "epiccommand" },
        .summary = "This command is epic"
    });
    commands->register_command(testTaskCommand, std::in_place,
        "testtask", "Test command using dpp::task");

    auto modules = std::make_unique<dppcmd::module_service>(cluster.get(), dppcmd::command_service_config {
        .command_prefix = '^',
        .throw_exceptions = true
    });

    modules->register_module<TestModule>();

    cluster->on_log(dpp::utility::cout_logger());
    cluster->on_message_create(std::bind_front(&on_message_create, commands.get(), modules.get()));

    cluster->start(dpp::st_wait);
}
