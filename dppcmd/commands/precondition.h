#pragma once
#include "dppcmd/results/preconditionresult.h"
#include "dppcmd/utils/ezcoro.h"

namespace dpp { class cluster; class message_create_t; }

namespace dppcmd
{
    class base_command_service;

    struct precondition
    {
        virtual TASK(precondition_result) check(dpp::cluster* cluster, const dpp::message_create_t* context,
                                                const base_command_service* service)
        { RETURN(precondition_result::from_success()); }
    };
}
