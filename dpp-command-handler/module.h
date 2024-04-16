#ifndef MODULE_H
#define MODULE_H
#include "commandinfo.h"
#include "readers/typereader.h"
#include "results/commandresult.h"
#include "utils/lexical_cast.h"
#include "utils/traits.h"
#include "commandfunction.h"
#include <deque>

class ModuleService;

class bad_command_argument : public std::exception
{
public:
    bad_command_argument(CommandError error, std::string_view arg, size_t argIndex, std::string_view module,
                         std::string_view command, std::string_view message);
    CommandError error() const { return m_error; }
    const char* what() const noexcept override { return m_message.c_str(); }
private:
    CommandError m_error;
    std::string m_message;
};

template<typename F>
concept MemberFunction = std::is_member_function_pointer_v<F>;

template<typename F>
concept NonMemberFunction = std::is_function_v<std::remove_pointer_t<F>>;

template<class Derived>
concept TypeReaderDerivative = requires(Derived& t) { []<typename X>(TypeReader<X>&){}(t); };

class ModuleBase
{
    friend class ModuleService;
public:
    explicit ModuleBase(std::string_view name, std::string_view summary = "") : m_name(name), m_summary(summary) {}
    virtual ~ModuleBase() = default;

    std::vector<std::reference_wrapper<const CommandInfo>> commands() const;
    std::string name() const { return m_name; }
    std::string summary() const { return m_summary; }

    friend inline std::ostream& operator<<(std::ostream& os, const ModuleBase& m) { return os << m.name(); }
protected:
    dpp::cluster* cluster;
    const dpp::message_create_t* context;
    const ModuleService* service;

    void registerCommand(MemberFunction auto&& f, auto&&... commandInfoArgs)
    {
        CommandInfo info(this, std::forward<decltype(commandInfoArgs)>(commandInfoArgs)...);
        auto mem_fn = std::mem_fn(f);
        using FTF = function_traits<decltype(mem_fn)>;
        using Args = tuple_tail<typename FTF::args>::type;
        using ModuleType = FTF::template arg<0>::type;
        using ResultType = FTF::result_type;

        auto func = std::make_unique<CommandFunction>();

#ifdef DPP_CORO
        if constexpr (dpp_task_type<ResultType>::value)
        {
            auto bufferFunc = [this, mem_fn, info](ModuleType module, std::deque<std::string>&& args) -> ResultType {
                auto fnArgs = std::tuple_cat(std::make_tuple(module), convertArgs<Args>(std::move(args), info.name()));
                using TaskResultType = dpp_task_type<ResultType>::value_type;
                if constexpr (std::is_same_v<TaskResultType, void>)
                    co_await std::apply(mem_fn, fnArgs);
                else
                    co_return co_await std::apply(mem_fn, fnArgs);
            };
            func->set(std::function<ResultType(ModuleType, std::deque<std::string>)>(bufferFunc), true, true);
        }
        else
        {
#endif
            auto bufferFunc = [this, mem_fn, info](ModuleType module, std::deque<std::string>&& args) -> ResultType {
                auto fnArgs = std::tuple_cat(std::make_tuple(module), convertArgs<Args>(std::move(args), info.name()));
                return std::apply(mem_fn, fnArgs);
            };
            func->set(std::function<ResultType(ModuleType, std::deque<std::string>)>(bufferFunc), true);
#ifdef DPP_CORO
        }
#endif

        func->setTargetArgCount(targetArgCount<Args>());
        m_commands.emplace_back(info, std::move(func));
    }

    void registerCommand(NonMemberFunction auto&& f, auto&&... commandInfoArgs)
    {
        CommandInfo info(this, std::forward<decltype(commandInfoArgs)>(commandInfoArgs)...);
        using FTF = function_traits<decltype(f)>;
        using Args = FTF::args;
        using ResultType = FTF::result_type;

        auto func = std::make_unique<CommandFunction>();

#ifdef DPP_CORO
        if constexpr (dpp_task_type<ResultType>::value)
        {
            auto bufferFunc = [this, f, info](std::deque<std::string>&& args) -> ResultType {
                using TaskResultType = dpp_task_type<ResultType>::value_type;
                if constexpr (std::is_same_v<TaskResultType, void>)
                    co_await std::apply(f, convertArgs<Args>(std::move(args), info.name()));
                else
                    co_return co_await std::apply(f, convertArgs<Args>(std::move(args), info.name()));
            };
            func->set(std::function<ResultType(std::deque<std::string>)>(bufferFunc), true, true);
        }
        else
        {
#endif
            auto bufferFunc = [this, f, info](std::deque<std::string>&& args) -> ResultType {
                return std::apply(f, convertArgs<Args>(std::move(args), info.name()));
            };
            func->set(std::function<ResultType(std::deque<std::string>)>(bufferFunc), true);
#ifdef DPP_CORO
        }
#endif

        func->setTargetArgCount(targetArgCount<Args>());
        m_commands.emplace_back(info, std::move(func));
    }
private:
    std::vector<std::pair<CommandInfo, std::unique_ptr<CommandFunction>>> m_commands;
    std::string m_name;
    std::string m_summary;

    template<typename, template<typename, typename...> typename>
    struct is_instance : std::false_type {};

    template<typename... Ts, template<typename, typename...> typename U>
    struct is_instance<U<Ts...>, U> : std::true_type {};

    template<typename T>
    struct tuple_tail {};

    template<typename T, typename... Ts>
    struct tuple_tail<std::tuple<T, Ts...>> { using type = std::tuple<Ts...>; };

    template<class Tuple>
    auto convertArgs(std::deque<std::string>&& args, std::string_view command)
    {
        return [this, args = std::move(args), &command]<size_t... Is>(std::index_sequence<Is...>) {
            return std::make_tuple(convertArg<std::tuple_element_t<Is, Tuple>>(args[Is], Is, command)...);
        }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
    }

    template<typename T>
    T convertArg(std::string_view arg, size_t argIndex, std::string_view command)
    {
        if constexpr (TypeReaderDerivative<T>)
        {
            T typeReader;
            TypeReaderResult result = typeReader.read(cluster, context, arg);
            if (!result.success())
                throw bad_command_argument(result.error().value(), arg, argIndex + 1, name(), command, result.message());
            return typeReader;
        }
        else if constexpr (is_instance<T, std::optional>::value)
        {
            if (arg.empty())
                return std::nullopt;
            return convertArg<typename T::value_type>(arg, argIndex, command);
        }
        else
        {
            try
            {
                return dpp::utility::lexical_cast<T>(arg);
            }
            catch (const dpp::utility::bad_lexical_cast& e)
            {
                throw bad_command_argument(CommandError::ParseFailed, arg, argIndex + 1, name(), command, e.what());
            }
        }
    }

    template<class Tuple>
    size_t targetArgCount()
    {
        if constexpr (std::tuple_size_v<Tuple> > 0)
        {
            return []<size_t... Is>(std::index_sequence<Is...>) {
                return (... + !is_instance<std::tuple_element_t<Is, Tuple>, std::optional>::value);
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
        }
        else
        {
            return 0;
        }
    }

    virtual TASK(CommandResult) createInstanceAndRun(CommandFunction* function, dpp::cluster* cluster,
                                                     const dpp::message_create_t* context, const ModuleService* service,
                                                     std::deque<std::string>&& args) = 0;
};

#define MODULE_SETUP(ModuleName) \
TASK(CommandResult) createInstanceAndRun(CommandFunction* function, dpp::cluster* cluster, \
                                         const dpp::message_create_t* context, const ModuleService* service, \
                                         std::deque<std::string>&& args) override \
{ \
    try \
    { \
        if (function->isMemberFunction()) \
        { \
            auto instance = std::make_unique<ModuleName>(); \
            instance->cluster = cluster; \
            instance->context = context; \
            instance->service = service; \
            if (function->isCoroutine()) \
                RETURN(AWAIT(function->operator()<TASK(CommandResult)>(instance.get(), std::move(args)))); \
            else \
                RETURN(function->operator()<CommandResult>(instance.get(), std::move(args))); \
        } \
        else \
        { \
            if (function->isCoroutine()) \
                RETURN(AWAIT(function->operator()<TASK(CommandResult)>(std::move(args)))); \
            else \
                RETURN(function->operator()<CommandResult>(std::move(args))); \
        } \
    } \
    catch (const bad_command_argument& e) \
    { \
        RETURN(CommandResult::fromError(e.error(), e.what())); \
    } \
    catch (const std::exception& e) \
    { \
        RETURN(CommandResult::fromError(e)); \
    } \
}

#endif // MODULE_H
