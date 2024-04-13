#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <deque>
#include <string>

namespace dpp
{
    namespace commandparser
    {
        std::deque<std::string> parseArguments(std::string_view input, char sep);
    }
}

#endif // COMMANDPARSER_H
