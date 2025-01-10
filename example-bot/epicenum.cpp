#include "epicenum.h"
#include "dppcmd/utils/strings.h"
#include <dpp/dispatcher.h>

dppcmd::type_reader_result EpicEnumReader::read(dpp::cluster* cluster,
    const dpp::message_create_t* context, const std::string& input)
{
    if (context->msg.author.username != "bowdown097")
        return dppcmd::type_reader_result::from_error(dppcmd::command_error::unsuccessful, "You aren't the right guy.");

    if (dppcmd::utility::iequals(input, "This"))
        add_result(EpicEnum::This);
    else if (dppcmd::utility::iequals(input, "Bot"))
        add_result(EpicEnum::Bot);
    else if (dppcmd::utility::iequals(input, "Is"))
        add_result(EpicEnum::Is);
    else if (dppcmd::utility::iequals(input, "Epic"))
        add_result(EpicEnum::Epic);
    else
        return dppcmd::type_reader_result::from_error(dppcmd::command_error::object_not_found, "You didn't say the right thing.");

    return dppcmd::type_reader_result::from_success();
}
