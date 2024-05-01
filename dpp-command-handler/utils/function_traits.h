#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

template <typename T>
struct function_traits_impl : function_traits_impl<decltype(&T::operator())> {};

template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(Args...)>
{
    enum { arity = sizeof...(Args) };

    typedef ReturnType result_type;
    typedef ReturnType function_type(Args...);
    typedef std::tuple<std::remove_cvref_t<Args>...> args;

    template <size_t I>
    struct arg
    {
        typedef std::tuple_element_t<I, args> type;
    };
};

template<typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(Args...) noexcept> : function_traits_impl<ReturnType(Args...)> {};

template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(*)(Args...)> : function_traits_impl<ReturnType(Args...)> {};

template<typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(*)(Args...) noexcept> : function_traits_impl<ReturnType(Args...)> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...)> : function_traits_impl<ReturnType(Args...)>
{
    typedef ClassType* owner_type;
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) noexcept> : function_traits_impl<ReturnType(Args...)>
{
    typedef ClassType* owner_type;
};

template <typename FunctionType>
struct function_traits_impl<std::function<FunctionType>> : function_traits_impl<FunctionType> {};

template<typename T>
struct function_traits : function_traits_impl<std::remove_cvref_t<T>> {};
