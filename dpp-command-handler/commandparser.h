#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <deque>
#include <string>

namespace CommandParser
{
    std::deque<std::string> parseArguments(const std::string& input, char sep);
}

#endif // COMMANDPARSER_H
