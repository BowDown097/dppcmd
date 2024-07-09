#pragma once
#include <type_traits>
#include <utility>

namespace dpp
{
    template<typename T>
    class remainder
    {
        static_assert(std::is_object_v<T> && !std::is_array_v<T>);
    public:
        using value_type = T;

        template<typename U = T> requires std::is_constructible_v<T, U> && std::is_convertible_v<U, T>
        constexpr remainder(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::forward<U>(value)) {}

        template<typename U = T> requires (!std::is_constructible_v<T, U> || std::is_convertible_v<U, T>)
        explicit constexpr remainder(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::forward<U>(value)) {}

        constexpr const T* operator->() const noexcept { return std::addressof(m_value); }
        constexpr T* operator->() noexcept { return std::addressof(m_value); }
        constexpr const T& operator*() const& noexcept { return m_value; }
        constexpr T& operator*() & noexcept { return m_value; }
        constexpr const T&& operator*() const&& noexcept { return std::move(m_value); }
        constexpr T&& operator*() && noexcept { return std::move(m_value); }
    private:
        T m_value;
    };
}
