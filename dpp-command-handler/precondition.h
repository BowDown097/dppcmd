#pragma once
#include "results/preconditionresult.h"
#include "utils/ezcoro.h"

namespace dpp
{
    class cluster;
    class message_create_t;
    class module_service;

    struct precondition
    {
        virtual TASK(precondition_result) check(cluster* cluster, const message_create_t* context,
                                                const module_service* service)
        { RETURN(precondition_result::from_success()); }
    };
}
