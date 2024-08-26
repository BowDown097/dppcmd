#pragma once
#include "typereader.h"
#include <cstdint>
#include <unordered_set>

namespace dpp { class snowflake; class user; }

namespace dppcmd
{
    class user_in : public type_reader<dpp::user*>
    {
    public:
        type_reader_result read(dpp::cluster* cluster, const dpp::message_create_t* context,
                                std::string_view input) override;
    protected:
        std::unordered_set<uint64_t> id_set;
        void add_results_by_global_name(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_id(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_mention(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_nickname(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_username(const dpp::snowflake guild_id, std::string_view input);
    };
}
