#ifndef CHANNELTYPEREADER_H
#define CHANNELTYPEREADER_H
#include "typereader.h"

namespace dpp { class channel; }

struct ChannelTypeReader : TypeReader<dpp::channel>
{
    TypeReaderResult read(dpp::cluster* cluster, const dpp::message_create_t* context, const std::string& input) override;
};

#endif // CHANNELTYPEREADER_H
