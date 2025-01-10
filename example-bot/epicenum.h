#pragma once
#include "dppcmd/readers/typereader.h"

enum class EpicEnum { This, Bot, Is, Epic };

class EpicEnumReader : public dppcmd::type_reader<EpicEnum>
{
public:
    dppcmd::type_reader_result read(dpp::cluster* cluster, const dpp::message_create_t* context,
                                    const std::string& input) override;
};
