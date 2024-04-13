#ifndef CHANNELTYPEREADER_H
#define CHANNELTYPEREADER_H
#include "typereader.h"

namespace dpp { class channel; }

struct ChannelTypeReader : TypeReader<dpp::channel>
{
    TypeReaderResult read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input) override;
};

#endif // CHANNELTYPEREADER_H
