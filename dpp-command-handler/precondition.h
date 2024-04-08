#ifndef PRECONDITION_H
#define PRECONDITION_H
#include "results/preconditionresult.h"
#include "utils/ezcoro.h"

class ModuleService;
namespace dpp { class cluster; class message_create_t; }

struct Precondition
{
    virtual TASK(PreconditionResult) check(dpp::cluster* cluster, const dpp::message_create_t* context,
                                           const ModuleService* service)
    { RETURN(PreconditionResult::fromSuccess()); }
};

#endif // PRECONDITION_H
