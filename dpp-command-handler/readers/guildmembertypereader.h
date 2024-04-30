#ifndef GUILDMEMBERTYPEREADER_H
#define GUILDMEMBERTYPEREADER_H
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
        void add_results_by_global_name(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_id(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_mention(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_nickname(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_username(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_username_and_discrim(const dpp::snowflake guild_id, std::string_view input);
    };
}

#endif // GUILDMEMBERTYPEREADER_H
