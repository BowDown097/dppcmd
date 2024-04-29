#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <deque>
#include <string>

namespace dpp
{
    namespace command_parser
    {
        std::deque<std::string> parse(std::string_view input, char sep);
    }
}

#endif // COMMANDPARSER_H
