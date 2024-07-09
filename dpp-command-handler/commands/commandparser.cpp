#include "commandparser.h"

namespace dpp
{
    namespace command_parser
    {
        enum class QuotationState { None, Single, Double };

        std::vector<std::string> parse(std::string_view input, char sep)
        {
            std::vector<std::string> out;

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
                            out.emplace_back();
                            whitespace = false;
                        }
                        if (escape)
                        {
                            if (c == '\\' || c == '"' || c == '\'' || c == sep)
                                out.back().push_back(c);
                            else
                                out.back().push_back('\\');
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
                            out.back().push_back(c);
                        }
                    }
                }
            }

            return out;
        }
    }
}
