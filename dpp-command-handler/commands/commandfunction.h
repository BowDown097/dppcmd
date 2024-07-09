#pragma once
#include <functional>
#include <memory>

#ifdef DPP_CORO
# include <dpp/coro/task.h>
#endif

namespace dpp
{
    template<typename>
    struct task_type : std::false_type {};

#ifdef DPP_CORO
    template<typename T>
    struct task_type<task<T>> : std::true_type { using value_type = T; };
#endif

    template<typename T>
    inline constexpr bool is_task_v = task_type<T>::value;

    struct variant_function_wrapper_base
    {
        virtual ~variant_function_wrapper_base() = default;
    };

    template<typename ReturnType, typename... Args>
    struct variant_function_wrapper : variant_function_wrapper_base
    {
        std::function<ReturnType(Args...)> func;
        explicit variant_function_wrapper(std::function<ReturnType(Args...)> f) : func(std::move(f)) {}
    };

    class command_function
    {
    public:
        bool is_coroutine() const { return m_is_coroutine; }
        void set_target_arg_count(size_t count) { m_target_arg_count = count; }
        size_t target_arg_count() const { return m_target_arg_count; }

        template<typename T>
        T invoke(auto&&... args)
        {
            using WrapperType = variant_function_wrapper<T, std::remove_cvref_t<decltype(args)>...>;
            if (WrapperType* fw = dynamic_cast<WrapperType*>(m_wrapper.get()))
                return fw->func(std::forward<decltype(args)>(args)...);
            return T{};
        }

        template<typename T> requires is_task_v<T>
        T invoke(auto&&... args)
        {
            using ValueType = task_type<T>::value_type;
            using WrapperType = variant_function_wrapper<T, std::remove_cvref_t<decltype(args)>...>;

            if (WrapperType* fw = dynamic_cast<WrapperType*>(m_wrapper.get()))
            {
                if constexpr (std::same_as<ValueType, void>)
                    co_await fw->func(std::forward<decltype(args)>(args)...);
                else
                    co_return co_await fw->func(std::forward<decltype(args)>(args)...);
            }

            co_return ValueType{};
        }

        template<typename ReturnType, typename... Args>
        void set(std::function<ReturnType(Args...)> f)
        {
            using WrapperType = variant_function_wrapper<ReturnType, Args...>;
            m_is_coroutine = task_type<ReturnType>::value;
            m_target_arg_count = sizeof...(Args);
            m_wrapper = std::make_unique<WrapperType>(f);
        }
    private:
        bool m_is_coroutine{};
        size_t m_target_arg_count{};
        std::unique_ptr<variant_function_wrapper_base> m_wrapper;
    };
}
