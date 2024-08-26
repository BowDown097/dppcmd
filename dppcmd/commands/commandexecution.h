#pragma once
#include "commandfunction.h"
#include "dppcmd/services/basecommandservice.h"
#include "dppcmd/utils/join.h"
#include "dppcmd/utils/tuple_traits.h"
#include "dppcmd/utils/type_traits.h"
#include "remainder.h"

#define BUFFER_PARAMS std::vector<std::string>&& args, const dpp::message_create_t* ctx, base_command_service* svc
#define BUFFER_TYPES std::vector<std::string>, const dpp::message_create_t*, base_command_service*

namespace dppcmd
{
    class module_base;

    class command_execution
    {
    public:
        template<typename T>
        static T convert_arg(std::string_view arg, size_t index, std::string_view cmd,
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
        static auto convert_arg_at(std::string_view cmd, std::span<const std::string> args,
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
        static auto convert_args(std::string_view cmd, std::vector<std::string>&& args,
                                 const dpp::message_create_t* ctx, base_command_service* svc)
        {
            return [args = std::move(args), cmd, ctx, svc]<size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(convert_arg_at<Tuple, Is>(cmd, args, ctx, svc)...);
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
        }

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
        static size_t target_arg_count()
        {
            if constexpr (std::tuple_size_v<Tuple> > 0)
            {
                return []<size_t... Is>(std::index_sequence<Is...>) {
                    return (... + (!utility::is_specialization_of_v<std::tuple_element_t<Is, Tuple>, std::optional> &&
                                   !std::same_as<std::tuple_element_t<Is, Tuple>, dpp::cluster*> &&
                                   !std::same_as<std::tuple_element_t<Is, Tuple>, dpp::message_create_t*>));
                }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
            }
            else
            {
                return 0;
            }
        }
    private:
        template<typename Result, typename Fn, typename Tuple> requires utility::is_task_v<Result>
        static Result apply_fn(Fn&& f, Tuple&& t)
        {
            if constexpr (std::same_as<typename utility::task_type<Result>::value_type, void>)
                co_await std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
            else
                co_return co_await std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
        }

        template<typename Result, typename Fn, typename Tuple>
        static Result apply_fn(Fn&& f, Tuple&& t)
        {
            return std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
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
            constexpr long contextIndex = utility::tuple_index_of_v<dpp::message_create_t*, Args>;
            if constexpr (clusterIndex == -1 && contextIndex == -1)
                return convert_args<Args>(std::move(args), cmd, ctx, svc);

            constexpr size_t drop = (clusterIndex != -1 && contextIndex != -1) ? 2 : 1;
            auto converted = convert_args<utility::tuple_drop_n_t<drop, Args>>(std::move(args), cmd, ctx, svc);

            if constexpr (clusterIndex == 0 && contextIndex == 1)
                return std::tuple_cat(std::make_tuple(svc->cluster(), ctx), converted);
            else if constexpr (clusterIndex == 0)
                return std::tuple_cat(std::make_tuple(svc->cluster()), converted);
            else if constexpr (contextIndex == 0)
                return std::tuple_cat(std::make_tuple(ctx, svc->cluster()), converted);
            else
                return std::tuple_cat(std::make_tuple(ctx), converted);
        }
    };
}
