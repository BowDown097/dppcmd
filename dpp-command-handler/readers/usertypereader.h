#ifndef USERTYPEREADER_H
#define USERTYPEREADER_H
#include "typereader.h"

namespace dpp
{
    class snowflake;
    class user;

    class user_in : public type_reader<user*>
    {
    public:
        type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) override;
    protected:
        void add_results_by_global_name(std::string_view input);
        void add_results_by_id(std::string_view input);
        void add_results_by_mention(std::string_view input);
        void add_results_by_nickname(const dpp::snowflake guild_id, std::string_view input);
        void add_results_by_username(std::string_view input);
        void add_results_by_username_and_discrim(std::string_view input);
    };
}

#endif // USERTYPEREADER_H
