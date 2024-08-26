#pragma once
#include <type_traits>

namespace dppcmd
{
    namespace utility
    {
        // from the (sadly rejected) proposal P2098, using std::remove_cvref_t
        template<class T, template<class...> class Primary>
        struct is_specialization_of : std::false_type {};
        template<template<class...> class Primary, class... Args>
        struct is_specialization_of<Primary<Args...>, Primary> : std::true_type {};
        template<class T, template<class...> class Primary>
        inline constexpr bool is_specialization_of_v = is_specialization_of<std::remove_cvref_t<T>, Primary>::value;
    }
}
