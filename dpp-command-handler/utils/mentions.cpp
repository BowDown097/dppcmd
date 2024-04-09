#include "mentions.h"
#include "lexical_cast.h"

namespace cmdhndlrutils
{
    namespace mentions
    {
        uint64_t parseChannel(const std::string& str)
        {
            if (str.size() >= 3 && str[0] == '<' && str[1] == '#' && str[str.size() - 1] == '>')
                return cmdhndlrutils::lexical_cast<uint64_t>(str.substr(2, str.size() - 3), false); // <#123>
            return 0;
        }

        uint64_t parseRole(const std::string& str)
        {
            if (str.size() >= 4 && str[0] == '<' && str[1] == '@' && str[2] == '&' && str[str.size() - 1] == '>')
                return cmdhndlrutils::lexical_cast<uint64_t>(str.substr(3, str.size() - 4), false); // <@&123>
            return 0;
        }

        uint64_t parseUser(const std::string& str)
        {
            if (str.size() >= 3 && str[0] == '<' && str[1] == '@' && str[str.size() - 1] == '>')
            {
                if (str.size() >= 4 && str[2] == '!')
                    return cmdhndlrutils::lexical_cast<uint64_t>(str.substr(3, str.size() - 4), false); // <@!123>
                else
                    return cmdhndlrutils::lexical_cast<uint64_t>(str.substr(2, str.size() - 3), false); // <@123>
            }

            return 0;
        }
    }
}
