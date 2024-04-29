#ifndef USERTYPEREADER_H
#define USERTYPEREADER_H
#include "typereader.h"

namespace dpp
{
    class user;

    struct user_in : type_reader<user>
    {
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    };
}

#endif // USERTYPEREADER_H
