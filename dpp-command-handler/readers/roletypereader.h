#ifndef ROLETYPEREADER_H
#define ROLETYPEREADER_H
#include "typereader.h"

namespace dpp
{
    class role;

    struct role_in : type_reader<role*>
    {
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    };
}

#endif // ROLETYPEREADER_H
