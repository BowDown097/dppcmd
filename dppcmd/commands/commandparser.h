#pragma once
#include <string>
#include <vector>

namespace dppcmd
{
    namespace command_parser
    {
        std::vector<std::string> parse(std::string_view input, char sep);
    }
}
