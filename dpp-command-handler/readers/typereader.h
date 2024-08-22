#pragma once
#include "dpp-command-handler/results/typereaderresult.h"
#include <span>
#include <vector>

namespace dpp
{
    class cluster;
    class message_create_t;

    template<typename T> requires std::is_object_v<T> && (!std::is_array_v<T>)
    class type_reader_value
    {
    public:
        using value_type = T;

        template<typename U = T> requires std::is_constructible_v<T, U>
        constexpr type_reader_value(U&& value, float weight) noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::forward<U>(value)), m_weight(weight) {}

        constexpr const T* operator->() const noexcept { return std::addressof(m_value); }
        constexpr T* operator->() noexcept { return std::addressof(m_value); }
        constexpr const T& operator*() const& noexcept { return m_value; }
        constexpr T& operator*() & noexcept { return m_value; }
        constexpr const T&& operator*() const&& noexcept { return std::move(m_value); }
        constexpr T&& operator*() && noexcept { return std::move(m_value); }

        constexpr const T& value() const& noexcept { return m_value; }
        constexpr T& value() & noexcept { return m_value; }
        constexpr const T&& value() const&& noexcept { return std::move(m_value); }
        constexpr T&& value() && noexcept { return std::move(m_value); }

        constexpr float weight() const { return m_weight; }
    private:
        T m_value;
        float m_weight;
    };

    template<typename T>
    class type_reader
    {
    public:
        using value_type = T;

        virtual type_reader_result read(cluster* cluster, const message_create_t* context, std::string_view input) = 0;

        decltype(auto) top_result() const
        {
            if (!has_result())
                throw std::logic_error("Tried to get top result from type reader with no results");

            const type_reader_value<T>* topValue{};
            for (const type_reader_value<T>& value : m_results)
                if (!topValue || topValue->weight() < value.weight())
                    topValue = &value;

            return topValue->value();
        }

        bool has_result() const { return !m_results.empty(); }
        explicit operator bool() const { return has_result(); }

        std::span<const type_reader_value<T>> results() const { return m_results; }
    protected:
        template<typename U = T> requires std::is_constructible_v<T, U>
        void add_result(U&& value, float weight = 1.0f) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            m_results.emplace_back(std::forward<U>(value), weight);
        }
    private:
        std::vector<type_reader_value<T>> m_results;
    };

    namespace utility
    {
        template<typename T>
        concept is_type_reader = requires(T& t) { []<typename X>(dpp::type_reader<X>&){}(t); };
    }
}
