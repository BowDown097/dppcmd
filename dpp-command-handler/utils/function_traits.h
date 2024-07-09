#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

namespace dpp
{
    namespace utility
    {
        template <typename T>
        struct function_traits_impl : function_traits_impl<decltype(&T::operator())> {};

        template <typename ReturnType, typename... Args>
        struct function_traits_impl<ReturnType(Args...)>
        {
            enum { arity = sizeof...(Args) };

            using result_type = ReturnType;
            using function_type = ReturnType(Args...);
            using args = std::tuple<std::remove_cvref_t<Args>...>;

            template <size_t I>
            using arg = std::tuple_element_t<I, args>;
        };

        template<typename ReturnType, typename... Args>
        struct function_traits_impl<ReturnType(Args...) noexcept>
            : function_traits_impl<ReturnType(Args...)> {};

        template <typename ReturnType, typename... Args>
        struct function_traits_impl<ReturnType(*)(Args...)>
            : function_traits_impl<ReturnType(Args...)> {};

        template<typename ReturnType, typename... Args>
        struct function_traits_impl<ReturnType(*)(Args...) noexcept>
            : function_traits_impl<ReturnType(Args...)> {};

        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits_impl<ReturnType(ClassType::*)(Args...)>
            : function_traits_impl<ReturnType(Args...)>
        {
            using owner_type = ClassType*;
        };

        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits_impl<ReturnType(ClassType::*)(Args...) noexcept>
            : function_traits_impl<ReturnType(Args...)>
        {
            using owner_type = ClassType*;
        };

        template <typename FunctionType>
        struct function_traits_impl<std::function<FunctionType>>
            : function_traits_impl<FunctionType> {};

        template<typename T>
        struct function_traits : function_traits_impl<std::remove_cvref_t<T>> {};
    }
}
