#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <string>
#include <vector>

namespace dpp
{
    namespace command_parser
    {
        std::vector<std::string> parse(std::string_view input, char sep);
    }
}

#endif // COMMANDPARSER_H
