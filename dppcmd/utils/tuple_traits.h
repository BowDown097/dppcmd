#pragma once
#include <tuple>

namespace dppcmd
{
    namespace utility
    {
        template<class T, class Tuple, class Idxs>
        struct tuple_index_of_impl;

        template<class T, class... Args, std::size_t... Is>
        struct tuple_index_of_impl<T, std::tuple<Args...>, std::index_sequence<Is...>>
        {
            static constexpr long value = []() {
                long result = -1;
                ((std::same_as<T, std::tuple_element_t<Is, std::tuple<Args...>>> && (result = Is)), ...);
                return result;
            }();
        };

        template<class T, class Tuple>
        struct tuple_index_of { static constexpr long value = -1; };

        template<class T>
        struct tuple_index_of<T, std::tuple<>> { static constexpr long value = -1; };

        template<class T, class... Args>
        struct tuple_index_of<T, std::tuple<Args...>> {
            using Tuple = std::tuple<std::remove_cv_t<Args>...>;
            static constexpr long value = tuple_index_of_impl<T, Tuple, std::index_sequence_for<Args...>>::value;
        };

        template<class T, class Tuple>
        constexpr long tuple_index_of_v = tuple_index_of<T, Tuple>::value;

        template<std::size_t N, class Tuple>
        struct tuple_drop_n;

        template<class Tuple>
        struct tuple_drop_n<0, Tuple>
        {
            using type = Tuple;
        };

        template<std::size_t N, typename T, typename... Ts> requires (N > 0)
        struct tuple_drop_n<N, std::tuple<T, Ts...>>
        {
            using type = typename tuple_drop_n<N - 1, std::tuple<Ts...>>::type;
        };

        template<std::size_t N, class Tuple>
        using tuple_drop_n_t = typename tuple_drop_n<N, Tuple>::type;

        template<std::size_t N, class Tuple>
        struct tuple_drop_nth;

        template<typename T, typename... Ts>
        struct tuple_drop_nth<0, std::tuple<T, Ts...>>
        {
            using type = std::tuple<Ts...>;
        };

        template<std::size_t N, typename T, typename... Ts>
        struct tuple_drop_nth<N, std::tuple<T, Ts...>>
        {
            using type = decltype(std::tuple_cat(
                std::declval<std::tuple<T>>(),
                std::declval<typename tuple_drop_nth<N - 1, std::tuple<Ts...>>::type>()));
        };

        template<std::size_t N, class Tuple>
        using tuple_drop_nth_t = typename tuple_drop_nth<N, Tuple>::type;
    }
}
