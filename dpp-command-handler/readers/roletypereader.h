#ifndef ROLETYPEREADER_H
#define ROLETYPEREADER_H
#include "typereader.h"

namespace dpp { class role; }

struct RoleTypeReader : TypeReader<dpp::role>
{
    TypeReaderResult read(dpp::cluster* cluster, const dpp::message_create_t* context, const std::string& input) override;
};

#endif // ROLETYPEREADER_H
