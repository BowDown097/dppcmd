#pragma once
#include "typereader.h"

namespace dpp
{
    class role;
    class snowflake;

    class role_in : public type_reader<role*>
    {
    public:
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    protected:
        void add_results_by_id(const snowflake guild_id, std::string_view input);
        void add_results_by_mention(const snowflake guild_id, std::string_view input);
        void add_results_by_name(const snowflake guild_id, std::string_view input);
    };
}
