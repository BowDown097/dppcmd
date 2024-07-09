#pragma once
#include "dpp-command-handler/results/preconditionresult.h"
#include "dpp-command-handler/utils/ezcoro.h"

namespace dpp
{
    class base_command_service;
    class cluster;
    class message_create_t;

    struct precondition
    {
        virtual TASK(precondition_result) check(cluster* cluster, const message_create_t* context,
                                                const base_command_service* service)
        { RETURN(precondition_result::from_success()); }
    };
}
