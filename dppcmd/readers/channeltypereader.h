#pragma once
#include "typereader.h"
#include <cstdint>
#include <unordered_set>

namespace dpp { class channel; class snowflake; }

namespace dppcmd
{
    class channel_in : public type_reader<dpp::channel*>
    {
    public:
        type_reader_result read(dpp::cluster* cluster, const dpp::message_create_t* context,
                                std::string_view input) override;
    protected:
        std::unordered_set<uint64_t> id_set;
        void add_results_by_id(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_mention(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_name(const dpp::snowflake guild_id, std::string_view input);
    };
}
