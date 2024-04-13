#ifndef USERTYPEREADER_H
#define USERTYPEREADER_H
#include "typereader.h"

namespace dpp { class user; }

struct UserTypeReader : TypeReader<dpp::user>
{
    TypeReaderResult read(dpp::cluster* cluster, const dpp::message_create_t* context, std::string_view input) override;
};

#endif // USERTYPEREADER_H
