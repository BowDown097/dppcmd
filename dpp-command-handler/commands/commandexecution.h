#pragma once
#include "commandfunction.h"
#include "dpp-command-handler/readers/typereader.h"
#include "dpp-command-handler/utils/join.h"
#include "dpp-command-handler/utils/tuple_traits.h"
#include "dpp-command-handler/utils/type_traits.h"
#include "remainder.h"

#define BUFFER_PARAMS std::vector<std::string>&& args, cluster* cluster, const message_create_t* context
#define BUFFER_TYPES std::vector<std::string>, cluster*, const message_create_t*

namespace dpp
{
    class cluster;
    class message_create_t;
    class module_base;

    class command_execution
    {
    public:
        template<typename T>
        static T convert_arg(std::string_view arg, size_t index, std::string_view cmd,
                             cluster* cluster, const message_create_t* context)
        {
            try
            {
                if constexpr (utility::is_type_reader<T>)
                {
                    T typeReader;
                    type_reader_result result = typeReader.read(cluster, context, arg);
                    if (!result.success())
                        throw bad_command_argument(result.error().value(), arg, index + 1, cmd, result.message());
                    return typeReader;
                }
                else if constexpr (utility::is_specialization_of_v<T, remainder>)
                {
                    return convert_arg<typename T::value_type>(arg, index, cmd, cluster, context);
                }
                else if constexpr (utility::is_specialization_of_v<T, std::optional>)
                {
                    if (arg.empty())
                        return std::nullopt;
                    return convert_arg<typename T::value_type>(arg, index, cmd, cluster, context);
                }
                else
                {
                    return utility::lexical_cast<T>(arg);
                }
            }
            catch (const utility::bad_lexical_cast& e)
            {
                throw bad_command_argument(command_error::parse_failed, arg, index + 1, cmd, e.what());
            }
        }

        template<class Tuple, size_t I>
        static auto convert_arg_at(std::span<const std::string> args, std::string_view cmd,
                                   cluster* cluster, const message_create_t* context)
        {
            using ArgType = std::tuple_element_t<I, Tuple>;
            if constexpr (utility::is_specialization_of_v<ArgType, remainder>)
            {
                return convert_arg<ArgType>(I < args.size() ? utility::join(args.subspan(I), ' ') : "",
                                            I, cmd, cluster, context);
            }
            else
            {
                return convert_arg<ArgType>(I < args.size() ? args[I] : "", I, cmd, cluster, context);
            }
        }

        template<class Tuple>
        static auto convert_args(std::vector<std::string>&& args, std::string_view cmd,
                                 cluster* cluster, const message_create_t* context)
        {
            return [args = std::move(args), cluster, context, cmd]<size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(convert_arg_at<Tuple, Is>(args, cmd, cluster, context)...);
            }(std::make_index_sequence<std::tuple_size_v<Tuple>>());
        }

        template<typename Result, typename Args, typename Module = void>
        static auto create_buffer_function(auto&& fn, const std::string& cmd)
        {
            if constexpr (std::derived_from<std::remove_pointer_t<Module>, module_base>)
            {
                return std::function<Result(Module, BUFFER_TYPES)>([cmd, fn](Module m, BUFFER_PARAMS) -> Result {
                    auto applyArgs = std::tuple_cat(std::make_tuple(m),
                        convert_args<Args>(std::move(args), cmd, cluster, context));
                    return apply_fn<Result>(fn, applyArgs);
                });
            }
            else
            {
                return std::function<Result(BUFFER_TYPES)>([cmd, fn](BUFFER_PARAMS) -> Result {
                    auto applyArgs = get_local_apply_args<Args>(cmd, std::move(args), cluster, context);
                    return apply_fn<Result>(fn, applyArgs);
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
        template<typename Result, typename Fn, typename Tuple> requires is_task_v<Result>
        static Result apply_fn(Fn&& f, Tuple&& t)
        {
            if constexpr (std::same_as<typename task_type<Result>::value_type, void>)
                co_await std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
            else
                co_return co_await std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
        }

        template<typename Result, typename Fn, typename Tuple>
        static Result apply_fn(Fn&& f, Tuple&& t)
        {
            return std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
        }

        template<typename Args>
        static auto get_local_apply_args(const std::string& cmd, BUFFER_PARAMS)
        {
            constexpr long clusterIndex = utility::tuple_index_of_v<dpp::cluster*, Args>;
            constexpr long contextIndex = utility::tuple_index_of_v<dpp::message_create_t*, Args>;
            if constexpr (clusterIndex == -1 && contextIndex == -1)
                return convert_args<Args>(std::move(args), cmd, cluster, context);

            decltype(auto) converted = clusterIndex != -1 && contextIndex != -1
                ? convert_args<utility::tuple_drop_two_t<Args>>(std::move(args), cmd, cluster, context)
                : convert_args<utility::tuple_drop_one_t<Args>>(std::move(args), cmd, cluster, context);

            if constexpr (clusterIndex == 0 && contextIndex == 1)
                return std::tuple_cat(std::make_tuple(cluster, context), converted);
            else if constexpr (clusterIndex == 0)
                return std::tuple_cat(std::make_tuple(cluster), converted);
            else if constexpr (contextIndex == 0)
                return std::tuple_cat(std::make_tuple(context, cluster), converted);
            else
                return std::tuple_cat(std::make_tuple(context), converted);
        }
    };
}
