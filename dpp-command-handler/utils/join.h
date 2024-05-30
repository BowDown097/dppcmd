#pragma once
#include "lexical_cast.h"
#include <functional>

namespace dpp
{
    namespace utility
    {
        template<std::ranges::range R, class F, class StringViewOrChar>
        requires std::same_as<std::invoke_result_t<F&, std::ranges::range_reference_t<R>>, std::string> &&
                 (std::is_convertible_v<StringViewOrChar, std::string_view> || std::same_as<StringViewOrChar, char>)
        std::string join(R&& range, StringViewOrChar delim, F&& func)
        {
            if (std::ranges::empty(range))
                return std::string();

            std::string init = std::invoke(func, *std::ranges::begin(range));
            for (auto it = std::next(std::ranges::begin(range)); it != std::ranges::end(range); ++it)
            {
                init += delim;
                init += std::invoke(func, *it);
            }

            return init;
        }

        template<class StringViewOrChar>
        requires std::is_convertible_v<StringViewOrChar, std::string_view> || std::same_as<StringViewOrChar, char>
        std::string join(std::ranges::range auto&& range, StringViewOrChar delim)
        {
            if (std::ranges::empty(range))
                return std::string();

            std::string init = lexical_cast<std::string>(*std::ranges::begin(range));
            for (auto it = std::next(std::ranges::begin(range)); it != std::ranges::end(range); ++it)
            {
                init += delim;
                init += lexical_cast<std::string>(*it);
            }

            return init;
        }
    }
}
