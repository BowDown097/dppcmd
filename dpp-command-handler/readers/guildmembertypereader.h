#pragma once
#include "typereader.h"

namespace dpp
{
    class guild_member;
    class snowflake;

    class guild_member_in : public type_reader<guild_member>
    {
    public:
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    protected:
        void add_results_by_global_name(const snowflake guild_id, std::string_view input);
        void add_results_by_id(const snowflake guild_id, std::string_view input);
        void add_results_by_mention(const snowflake guild_id, std::string_view input);
        void add_results_by_nickname(const snowflake guild_id, std::string_view input);
        void add_results_by_username(const snowflake guild_id, std::string_view input);
    };
}
