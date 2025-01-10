#include "commandexecution.h"
#include "dppcmd/services/commandservice.h"

namespace dppcmd
{
    command_service* command_execution::create_command_service(base_command_service* svc)
    {
        return dynamic_cast<command_service*>(svc);
    }
}
