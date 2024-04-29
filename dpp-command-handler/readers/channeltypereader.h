#ifndef CHANNELTYPEREADER_H
#define CHANNELTYPEREADER_H
#include "typereader.h"

namespace dpp
{
    class channel;

    struct channel_in : type_reader<channel*>
    {
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    };
}

#endif // CHANNELTYPEREADER_H
