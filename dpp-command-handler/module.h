#pragma once
#include "commandfunction.h"
#include "commandinfo.h"
#include "readers/typereader.h"
#include "results/commandresult.h"
#include "utils/function_traits.h"
#include "utils/join.h"
#include "utils/strings.h"
#include "utils/type_traits.h"
#include <any>

namespace dpp
{
    class module_service;

    class bad_command_argument : public std::exception
    {
    public:
        bad_command_argument(command_error error, std::string_view arg, size_t index, std::string_view module,
                             std::string_view command, std::string_view message);
        command_error error() const { return m_error; }
        const char* what() const noexcept override { return m_message.c_str(); }
    private:
        command_error m_error;
        std::string m_message;
    };

    template<typename T>
    class remainder
    {
    public:
        using value_type = T;

        template<typename U = T> requires std::is_constructible_v<T, U> && std::is_convertible_v<U, T>
        constexpr remainder(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::move(value)) {}

        template<typename U = T> requires (!(std::is_constructible_v<T, U> && !std::is_convertible_v<U, T>))
        explicit constexpr remainder(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::move(value)) {}

        constexpr const T* operator->() const noexcept { return std::addressof(m_value); }
        constexpr T* operator->() noexcept { return std::addressof(m_value); }
        constexpr const T& operator*() const& noexcept { return m_value; }
        constexpr T& operator*() & noexcept { return m_value; }
        constexpr const T&& operator*() const&& noexcept { return std::move(m_value); }
        constexpr T&& operator*() && noexcept { return std::move(m_value); }
    private:
        T m_value;
    };

    class module_base
    {
        friend class module_service;
    public:
        explicit module_base(std::string_view name, std::string_view summary = "") : m_name(name), m_summary(summary) {}
        virtual ~module_base() = default;

        std::vector<std::reference_wrapper<const command_info>> commands() const;
        const std::string& name() const { return m_name; }
        const std::string& summary() const { return m_summary; }

        friend inline std::ostream& operator<<(std::ostream& os, const module_base& m) { return os << m.name(); }
    protected:
        dpp::cluster* cluster{};
        const message_create_t* context{};
        const module_service* service{};

        template<typename MemberFunction> requires std::is_member_function_pointer_v<MemberFunction>
        void register_command(MemberFunction f, auto&&... command_info_args)
        {
            command_info info(this, std::forward<decltype(command_info_args)>(command_info_args)...);
            auto mem_fn = std::mem_fn(f);
            using FTF = function_traits<MemberFunction>;
            using Args = FTF::args;
            using ModuleType = FTF::owner_type;
            using ResultType = FTF::result_type;

            auto func = std::make_unique<command_function>();

    #ifdef DPP_CORO
            if constexpr (task_type<ResultType>::value)
            {
                auto bufferFunc = [this, mem_fn, info](ModuleType module, std::vector<std::string>&& args) -> ResultType {
                    auto fnArgs = std::tuple_cat(std::make_tuple(module),
                        module->template convert_args<Args>(std::move(args), info.name()));
                    if constexpr (std::is_same_v<typename task_type<ResultType>::value_type, void>)
                        co_await std::apply(mem_fn, fnArgs);
                    else
                        co_return co_await std::apply(mem_fn, fnArgs);
                };
                func->set(std::function<ResultType(ModuleType, std::vector<std::string>)>(bufferFunc), true);
            }
            else
            {
    #endif
                auto bufferFunc = [this, mem_fn, info](ModuleType module, std::vector<std::string>&& args) -> ResultType {
                    auto fnArgs = std::tuple_cat(std::make_tuple(module),
                        module->template convert_args<Args>(std::move(args), info.name()));
                    return std::apply(mem_fn, fnArgs);
                };
                func->set(std::function<ResultType(ModuleType, std::vector<std::string>)>(bufferFunc));
    #ifdef DPP_CORO
            }
    #endif

            func->set_target_arg_count(target_arg_count<Args>());
            m_commands.emplace_back(info, std::move(func));
        }
    private:
        std::vector<std::pair<command_info, std::unique_ptr<command_function>>> m_commands;
        std::string m_name;
        std::string m_summary;

        template<class Tuple>
        auto convert_args(const std::span<const std::string>& args, std::string_view cmd)
        {
            return [this, &args, &cmd]<size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(convert_arg_at<Tuple, Is>(args, cmd)...);
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
        }

        template<class Tuple, size_t I>
        auto convert_arg_at(const std::span<const std::string>& args, std::string_view cmd)
        {
            using ArgType = std::tuple_element_t<I, Tuple>;
            if constexpr (utility::is_specialization_of_v<ArgType, remainder>)
                return convert_arg<ArgType>(I < args.size() ? utility::join(args.subspan(I), ' ') : "", I, cmd);
            else
                return convert_arg<ArgType>(I < args.size() ? args[I] : "", I, cmd);
        }

        template<typename T>
        T convert_arg(std::string_view arg, size_t index, std::string_view cmd)
        {
            if constexpr (utility::is_type_reader<T>)
            {
                T typeReader;
                type_reader_result result = typeReader.read(cluster, context, arg);
                if (!result.success())
                    throw bad_command_argument(result.error().value(), arg, index + 1, name(), cmd, result.message());
                return typeReader;
            }
            else if constexpr (utility::is_specialization_of_v<T, remainder>)
            {
                return convert_arg<typename T::value_type>(arg, index, cmd);
            }
            else if constexpr (utility::is_specialization_of_v<T, std::optional>)
            {
                if (arg.empty())
                    return std::nullopt;
                return convert_arg<typename T::value_type>(arg, index, cmd);
            }
            else
            {
                try
                {
                    return utility::lexical_cast<T>(arg);
                }
                catch (const utility::bad_lexical_cast& e)
                {
                    throw bad_command_argument(command_error::parse_failed, arg, index + 1, name(), cmd, e.what());
                }
            }
        }

        template<class Tuple>
        size_t target_arg_count()
        {
            if constexpr (std::tuple_size_v<Tuple> > 0)
            {
                return []<size_t... Is>(std::index_sequence<Is...>) {
                    return (... + !utility::is_specialization_of_v<std::tuple_element_t<Is, Tuple>, std::optional>);
                }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
            }
            else
            {
                return 0;
            }
        }

        virtual TASK(command_result) exec(command_function* function, dpp::cluster* cluster,
            const message_create_t* context, const module_service* service, std::vector<std::string>&& args,
            const std::any& extra_data = {}) = 0;
    };

    template<typename Derived>
    class module : public module_base
    {
    public:
        explicit module(std::string_view name, std::string_view summary = "")
            : module_base(name, summary) {}

        TASK(command_result) exec(command_function* function, dpp::cluster* cluster,
            const message_create_t* context, const module_service* service, std::vector<std::string>&& args,
            const std::any& extra_data = {}) override
        {
            try
            {
                auto instance = std::make_unique<Derived>();
                instance->cluster = cluster;
                instance->context = context;
                instance->m_extra_data = extra_data;
                instance->service = service;

                if (function->is_coroutine())
                    RETURN(AWAIT(function->operator()<TASK(command_result)>(instance.get(), std::move(args))));
                else
                    RETURN(function->operator()<command_result>(instance.get(), std::move(args)));
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
    protected:
        template<typename T>
        T extra_data() const
        {
            if (!m_extra_data.has_value())
            {
                throw std::logic_error("Attempted to get extra data for module " +
                                       utility::demangle(typeid(Derived).name()) +
                                       " when no such data exists");
            }

            return std::any_cast<T>(m_extra_data);
        }
    private:
        std::any m_extra_data;
    };
}
