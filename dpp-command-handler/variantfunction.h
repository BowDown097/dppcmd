#ifndef VARIANTFUNCTION_H
#define VARIANTFUNCTION_H
#include <functional>
#include <memory>

#ifdef DPP_CORO
# include <dpp/coro/task.h>

template<typename>
struct dpp_task_type : std::false_type {};

template<typename T>
struct dpp_task_type<dpp::task<T>> : std::true_type { using result_type = T; };

template<typename T>
concept DppTask = dpp_task_type<T>::value;
#endif

struct VariantFunctionWrapperBase
{
    virtual ~VariantFunctionWrapperBase() = default;
};

template<typename ReturnType, typename... Args>
struct VariantFunctionWrapper : VariantFunctionWrapperBase
{
    std::function<ReturnType(Args...)> func;
};

class VariantFunction
{
public:
    bool isCoroutine() const { return m_isCoroutine; }
    bool isMemberFunction() const { return m_isMemberFunction; }

    void setTargetArgCount(size_t targetArgCount) { m_targetArgCount = targetArgCount; }
    size_t targetArgCount() const { return m_targetArgCount; }

    template<typename T>
    T operator()(auto&&... args)
    {
        using WrapperType = VariantFunctionWrapper<T, std::decay_t<decltype(args)>...>;
        WrapperType* fw = dynamic_cast<WrapperType*>(m_wrapper.get());
        if (fw)
            return fw->func(std::forward<decltype(args)>(args)...);
        else if constexpr (std::is_default_constructible_v<T>)
            return T{};
    }

#ifdef DPP_CORO
    template<DppTask T>
    T operator()(auto&&... args)
    {
        using ResultType = dpp_task_type<T>::result_type;
        using WrapperType = VariantFunctionWrapper<T, std::decay_t<decltype(args)>...>;
        WrapperType* fw = dynamic_cast<WrapperType*>(m_wrapper.get());
        if (fw)
        {
            if constexpr (std::is_same_v<ResultType, void>)
                co_await fw->func(std::forward<decltype(args)>(args)...);
            else
                co_return co_await fw->func(std::forward<decltype(args)>(args)...);
        }
        else if constexpr (std::is_default_constructible_v<ResultType>)
        {
            co_return ResultType{};
        }
    }
#endif

    template<typename ReturnType, typename... Args>
    void set(std::function<ReturnType(Args...)> f, bool isMemberFunction, bool isCoroutine = false)
    {
        using WrapperType = VariantFunctionWrapper<ReturnType, Args...>;
        m_isCoroutine = isCoroutine;
        m_isMemberFunction = isMemberFunction;
        m_targetArgCount = sizeof...(Args);
        m_wrapper = std::make_unique<WrapperType>();
        dynamic_cast<WrapperType*>(m_wrapper.get())->func = f;
    }
private:
    bool m_isCoroutine;
    bool m_isMemberFunction;
    size_t m_targetArgCount;
    std::unique_ptr<VariantFunctionWrapperBase> m_wrapper;
};

#endif // VARIANTFUNCTION_H
