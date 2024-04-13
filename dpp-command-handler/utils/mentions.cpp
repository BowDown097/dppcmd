#include "mentions.h"
#include "lexical_cast.h"

namespace dpp
{
    namespace utility
    {
        uint64_t parse_channel_mention(std::string_view str)
        {
            if (str.size() >= 3 && str[0] == '<' && str[1] == '#' && str[str.size() - 1] == '>')
                return dpp::utility::lexical_cast<uint64_t>(str.substr(2, str.size() - 3), false); // <#123>
            return 0;
        }

        uint64_t parse_role_mention(std::string_view str)
        {
            if (str.size() >= 4 && str[0] == '<' && str[1] == '@' && str[2] == '&' && str[str.size() - 1] == '>')
                return dpp::utility::lexical_cast<uint64_t>(str.substr(3, str.size() - 4), false); // <@&123>
            return 0;
        }

        uint64_t parse_user_mention(std::string_view str)
        {
            if (str.size() >= 3 && str[0] == '<' && str[1] == '@' && str[str.size() - 1] == '>')
            {
                if (str.size() >= 4 && str[2] == '!')
                    return dpp::utility::lexical_cast<uint64_t>(str.substr(3, str.size() - 4), false); // <@!123>
                else
                    return dpp::utility::lexical_cast<uint64_t>(str.substr(2, str.size() - 3), false); // <@123>
            }

            return 0;
        }
    }
}
