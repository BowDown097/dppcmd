#pragma once
#include <cstddef>
#include <tuple>

namespace dpp
{
    namespace utility
    {
        template<class T, class Tuple, class Idxs>
        struct tuple_index_of_impl;

        template<class T, class... Args, size_t... Is>
        struct tuple_index_of_impl<T, std::tuple<Args...>, std::index_sequence<Is...>>
        {
            static constexpr size_t value = []() {
                size_t result = -1;
                ((std::same_as<T, std::tuple_element_t<Is, std::tuple<Args...>>> && (result = Is)), ...);
                return result;
            }();
        };

        template<class T, class Tuple>
        struct tuple_index_of { static constexpr size_t value = -1; };

        template<class T>
        struct tuple_index_of<T, std::tuple<>> { static constexpr size_t value = -1; };

        template<class T, class... Args>
        struct tuple_index_of<T, std::tuple<Args...>> {
            using Tuple = std::tuple<std::remove_cv_t<Args>...>;
            static constexpr size_t value = tuple_index_of_impl<T, Tuple, std::index_sequence_for<Args...>>::value;
        };

        template<class T, class Tuple>
        constexpr size_t tuple_index_of_v = tuple_index_of<T, Tuple>::value;

        template<size_t N, class Tuple>
        struct tuple_drop_nth {};

        template<typename T, typename... Ts>
        struct tuple_drop_nth<0, std::tuple<T, Ts...>>
        {
            using type = std::tuple<Ts...>;
        };

        template<size_t N, typename T, typename... Ts>
        struct tuple_drop_nth<N, std::tuple<T, Ts...>>
        {
            using type = decltype(std::tuple_cat(
                std::declval<std::tuple<T>>(),
                std::declval<typename tuple_drop_nth<N - 1, std::tuple<Ts...>>::type>()));
        };

        template<size_t N, class Tuple>
        using tuple_drop_nth_t = typename tuple_drop_nth<N, Tuple>::type;

        template<class Tuple>
        using tuple_drop_one_t = typename tuple_drop_nth<0, Tuple>::type;

        template<class Tuple>
        using tuple_drop_two_t = tuple_drop_one_t<tuple_drop_one_t<Tuple>>;
    }
}
