#pragma once
#include <algorithm>

namespace dpp
{
    namespace utility
    {
        template<class T, class Tuple>
        struct tuple_index_of { static constexpr size_t value = -1; };

        template<class T, typename... Ts>
        struct tuple_index_of<T, std::tuple<Ts...>>
        {
            static constexpr size_t value = []() {
                constexpr std::array<bool, sizeof...(Ts)> a{{ std::same_as<T, std::remove_cv_t<Ts>>... }};
                const auto it = std::ranges::find(a, true);
                if (it == a.end())
                    return static_cast<size_t>(-1);
                return static_cast<size_t>(std::distance(a.begin(), it));
            }();
        };

        template<class T, class Tuple>
        inline constexpr bool tuple_index_of_v = tuple_index_of<T, Tuple>::value;
    }
}
