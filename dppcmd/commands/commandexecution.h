#pragma once
#include "dppcmd/services/basecommandservice.h"
#include "dppcmd/utils/join.h"
#include "dppcmd/utils/tuple_traits.h"
#include "dppcmd/utils/type_traits.h"
#include "exceptions.h"
#include "remainder.h"

#define BUFFER_PARAMS std::vector<std::string>&& args, const dpp::message_create_t* ctx, base_command_service* svc
#define BUFFER_TYPES std::vector<std::string>, const dpp::message_create_t*, base_command_service*

namespace dppcmd
{
    class command_service;
    class module_base;

    class command_execution
    {
    public:
        template<typename Result, typename Args, typename Module = void>
        static auto create_buffer_function(auto&& fn, const std::string& cmd)
        {
            if constexpr (std::derived_from<std::remove_pointer_t<Module>, module_base>)
            {
                return std::function<Result(Module, BUFFER_TYPES)>([cmd, fn](Module m, BUFFER_PARAMS) -> Result {
                    auto fn_args = get_apply_args<Args>(m, cmd, std::move(args), ctx, svc);
                    return apply_fn<Result>(fn, fn_args);
                });
            }
            else
            {
                return std::function<Result(BUFFER_TYPES)>([cmd, fn](BUFFER_PARAMS) -> Result {
                    auto fn_args = get_apply_args<Args>(cmd, std::move(args), ctx, svc);
                    return apply_fn<Result>(fn, fn_args);
                });
            }
        }

        template<class Tuple>
        static constexpr size_t target_arg_count()
        {
            if constexpr (std::tuple_size_v<Tuple> > 0)
            {
                return []<size_t... Is>(std::index_sequence<Is...>) {
                    return (... + []{
                        using ArgType = std::tuple_element_t<Is, Tuple>;
                        return !utility::is_specialization_of_v<ArgType, std::optional> &&
                               !std::same_as<ArgType, dpp::cluster*> &&
                               !std::same_as<ArgType, const dpp::message_create_t*> &&
                               !std::same_as<ArgType, const command_service*>;
                    }());
                }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
            }
            else
            {
                return 0;
            }
        }
    private:
        // this exists because dynamic_cast requires a complete type definition,
        // which would cause a recursive include, as commandservice.h includes this file.
        static command_service* create_command_service(base_command_service* svc);

    #ifdef DPP_CORO
        template<typename Result, typename Fn, typename Tuple>
            requires utility::is_specialization_of_v<Result, dpp::task>
        static Result apply_fn(Fn&& f, Tuple&& t)
        {
            if constexpr (std::same_as<dpp::detail::awaitable_result<Result>, void>)
                co_await std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
            else
                co_return co_await std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
        }
    #endif

        template<typename Result, typename Fn, typename Tuple>
        static Result apply_fn(Fn&& f, Tuple&& t)
        {
            return std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
        }

        template<typename T>
        static T convert_arg(const std::string& arg, size_t index, const std::string& cmd,
                             const dpp::message_create_t* ctx, base_command_service* svc)
        {
            try
            {
                if constexpr (utility::is_type_reader<T>)
                {
                    T reader;
                    if (type_reader_result result = reader.read(svc->cluster(), ctx, arg); !result.success())
                        throw bad_command_argument(result.error().value(), arg, index + 1, cmd, result.message());
                    return reader;
                }
                else if constexpr (utility::is_specialization_of_v<T, remainder>)
                {
                    return convert_arg<typename T::value_type>(arg, index, cmd, ctx, svc);
                }
                else if constexpr (utility::is_specialization_of_v<T, std::optional>)
                {
                    if (arg.empty())
                        return std::nullopt;
                    return convert_arg<typename T::value_type>(arg, index, cmd, ctx, svc);
                }
                else
                {
                    if (std::unique_ptr<type_reader<T>> reader = svc->create_type_reader<T>())
                    {
                        if (type_reader_result result = reader->read(svc->cluster(), ctx, arg); !result.success())
                            throw bad_command_argument(result.error().value(), arg, index + 1, cmd, result.message());
                        return reader->top_result();
                    }

                    // this fixes compile errors with types that have a type reader
                    // that cannot be lexical_casted (streamed)
                    if constexpr (requires(T t, std::istream& is) { is >> t; })
                        return utility::lexical_cast<T>(arg);
                    else
                        throw utility::bad_lexical_cast("std::string_view", typeid(T).name());
                }
            }
            catch (const utility::bad_lexical_cast& e)
            {
                throw bad_command_argument(command_error::parse_failed, arg, index + 1, cmd, e.what());
            }
        }

        template<class Tuple, size_t I>
        static auto convert_arg_at(const std::string& cmd, std::span<const std::string> args,
                                   const dpp::message_create_t* ctx, base_command_service* svc)
        {
            using ArgType = std::tuple_element_t<I, Tuple>;
            if (I >= args.size()) // still using convert_arg if index is OOB, may be an optional argument
                return convert_arg<ArgType>("", I, cmd, ctx, svc);

            if constexpr (utility::is_specialization_of_v<ArgType, remainder>)
                return convert_arg<ArgType>(utility::join(args.subspan(I), ' '), I, cmd, ctx, svc);
            else
                return convert_arg<ArgType>(args[I], I, cmd, ctx, svc);
        }

        template<class Tuple>
        static auto convert_args(const std::string& cmd, std::vector<std::string>&& args,
                                 const dpp::message_create_t* ctx, base_command_service* svc)
        {
            return [args = std::move(args), &cmd, ctx, svc]<size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(convert_arg_at<Tuple, Is>(cmd, args, ctx, svc)...);
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
        }

        template<typename Args, typename Module>
        static auto get_apply_args(Module m, const std::string& cmd, BUFFER_PARAMS)
        {
            return std::tuple_cat(std::make_tuple(m), convert_args<Args>(cmd, std::move(args), ctx, svc));
        }

        template<typename Args>
        static auto get_apply_args(const std::string& cmd, BUFFER_PARAMS)
        {
            constexpr long clusterIndex = utility::tuple_index_of_v<dpp::cluster*, Args>;
            constexpr long contextIndex = utility::tuple_index_of_v<const dpp::message_create_t*, Args>;
            constexpr long serviceIndex = utility::tuple_index_of_v<const command_service*, Args>;

            constexpr size_t drop = (clusterIndex != -1) + (contextIndex != -1) + (serviceIndex != -1);
            if constexpr (drop == 0)
                return convert_args<Args>(cmd, std::move(args), ctx, svc);

            auto converted = convert_args<utility::tuple_drop_n_t<drop, Args>>(cmd, std::move(args), ctx, svc);
            if constexpr (clusterIndex == 0 && contextIndex == 1 && serviceIndex == 2)
                return std::tuple_cat(std::make_tuple(svc->cluster(), ctx, create_command_service(svc)), std::move(converted));
            else if constexpr (clusterIndex == 0 && contextIndex == 2 && serviceIndex == 1)
                return std::tuple_cat(std::make_tuple(svc->cluster(), create_command_service(svc), ctx), std::move(converted));
            else if constexpr (clusterIndex == 0 && contextIndex == 1 && serviceIndex == -1)
                return std::tuple_cat(std::make_tuple(svc->cluster(), ctx), std::move(converted));
            else if constexpr (clusterIndex == 0 && contextIndex == -1 && serviceIndex == 1)
                return std::tuple_cat(std::make_tuple(svc->cluster(), create_command_service(svc)), std::move(converted));
            else if constexpr (clusterIndex == 1 && contextIndex == 0 && serviceIndex == 2)
                return std::tuple_cat(std::make_tuple(ctx, svc->cluster(), create_command_service(svc)), std::move(converted));
            else if constexpr (clusterIndex == 1 && contextIndex == 2 && serviceIndex == 0)
                return std::tuple_cat(std::make_tuple(create_command_service(svc), svc->cluster(), ctx), std::move(converted));
            else if constexpr (clusterIndex == 1 && contextIndex == 0 && serviceIndex == -1)
                return std::tuple_cat(std::make_tuple(ctx, svc->cluster()), std::move(converted));
            else if constexpr (clusterIndex == 1 && contextIndex == -1 && serviceIndex == 0)
                return std::tuple_cat(std::make_tuple(create_command_service(svc), svc->cluster()), std::move(converted));
            else if constexpr (clusterIndex == 2 && contextIndex == 0 && serviceIndex == 1)
                return std::tuple_cat(std::make_tuple(ctx, create_command_service(svc), svc->cluster()), std::move(converted));
            else if constexpr (clusterIndex == 2 && contextIndex == 1 && serviceIndex == 0)
                return std::tuple_cat(std::make_tuple(create_command_service(svc), ctx, svc->cluster()), std::move(converted));
            else if constexpr (clusterIndex == -1 && contextIndex == 0 && serviceIndex == 1)
                return std::tuple_cat(std::make_tuple(ctx, create_command_service(svc)), std::move(converted));
            else if constexpr (clusterIndex == -1 && contextIndex == 1 && serviceIndex == 0)
                return std::tuple_cat(std::make_tuple(create_command_service(svc), ctx), std::move(converted));
            else if constexpr (clusterIndex == 0 && contextIndex == -1 && serviceIndex == -1)
                return std::tuple_cat(std::make_tuple(svc->cluster()), std::move(converted));
            else if constexpr (clusterIndex == -1 && contextIndex == 0 && serviceIndex == -1)
                return std::tuple_cat(std::make_tuple(ctx), std::move(converted));
            else if constexpr (clusterIndex == -1 && contextIndex == -1 && serviceIndex == 0)
                return std::tuple_cat(std::make_tuple(create_command_service(svc)), std::move(converted));
        }
    };
}
