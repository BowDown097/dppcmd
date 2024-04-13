#include "commandparser.h"

// skidded from lenny because i got lazy lmao
namespace dpp
{
    namespace commandparser
    {
        enum class QuotationState { None, Single, Double };

        std::deque<std::string> parseArguments(std::string_view input, char sep)
        {
            std::deque<std::string> output;

            bool whitespace = true, escape = false;
            QuotationState quoteState = QuotationState::None;

            for (const char& c : input)
            {
                if (c == sep && quoteState == QuotationState::None && !escape)
                {
                    whitespace = true;
                }
                else
                {
                    if (!escape && c == '\\')
                    {
                        escape = true;
                    }
                    else
                    {
                        if (whitespace)
                        {
                            output.emplace_back();
                            whitespace = false;
                        }
                        if (escape)
                        {
                            if (c == '\\' || c == '"' || c == '\'' || c == sep)
                                output.back().push_back(c);
                            else
                                output.back().push_back('\\');
                        }
                        else if (quoteState != QuotationState::Single && c == '"')
                        {
                            quoteState = quoteState == QuotationState::None ? QuotationState::Double : QuotationState::None;
                        }
                        else if (quoteState != QuotationState::Double && c == '\'')
                        {
                            quoteState = quoteState == QuotationState::None ? QuotationState::Single : QuotationState::None;
                        }
                        else
                        {
                            output.back().push_back(c);
                        }
                    }
                }
            }

            return output;
        }
    }
}
