#pragma once
#include "dppcmd/results/commandresult.h"
#include "dppcmd/utils/ezcoro.h"
#include "dppcmd/utils/type_traits.h"
#include "exceptions.h"
#include <functional>
#include <memory>

namespace dppcmd
{
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
        void set_target_arg_count(size_t count) { m_target_arg_count = count; }

        TASK(command_result) invoke_with_result(std::string_view name, size_t arg_count, bool exceptions, auto&&... args)
        {
            if (arg_count < m_target_arg_count)
            {
                bad_argument_count arg_ex(name, arg_count, m_target_arg_count);
                if (exceptions)
                    throw arg_ex;
                else
                    RETURN(command_result::from_error(arg_ex.error(), arg_ex.what()));
            }

            if (exceptions)
            {
            #ifdef DPP_CORO
                if (m_is_task)
                    RETURN(AWAIT(invoke<TASK(command_result)>(std::forward<decltype(args)>(args)...)));
                else
            #endif
                    RETURN(invoke<command_result>(std::forward<decltype(args)>(args)...));
            }
            else
            {
                try
                {
                #ifdef DPP_CORO
                    if (m_is_task)
                        RETURN(AWAIT(invoke<TASK(command_result)>(std::forward<decltype(args)>(args)...)));
                    else
                #endif
                        RETURN(invoke<command_result>(std::forward<decltype(args)>(args)...));
                }
                catch (const bad_command_argument& e)
                {
                    RETURN(command_result::from_error(e.error(), e.what()));
                }
                catch (const std::exception& e)
                {
                    RETURN(command_result::from_error(e));
                }
            }
        }

        template<typename ReturnType, typename... Args>
        void set(std::function<ReturnType(Args...)> f)
        {
        #ifdef DPP_CORO
            m_is_task = utility::is_specialization_of_v<ReturnType, dpp::task>;
        #endif

            using WrapperType = variant_function_wrapper<ReturnType, Args...>;
            m_target_arg_count = sizeof...(Args);
            m_wrapper = std::make_unique<WrapperType>(std::move(f));
        }
    private:
    #ifdef DPP_CORO
        bool m_is_task{};
    #endif
        size_t m_target_arg_count{};
        std::unique_ptr<variant_function_wrapper_base> m_wrapper;

        template<typename T>
        T invoke(auto&&... args)
        {
            using WrapperType = variant_function_wrapper<T, std::remove_cvref_t<decltype(args)>...>;
            if (WrapperType* fw = dynamic_cast<WrapperType*>(m_wrapper.get()))
                return fw->func(std::forward<decltype(args)>(args)...);
            return T{};
        }

    #ifdef DPP_CORO
        template<typename T> requires utility::is_specialization_of_v<T, dpp::task>
        T invoke(auto&&... args)
        {
            using ValueType = dpp::detail::awaitable_result<T>;
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
    #endif
    };
}
