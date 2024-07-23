#pragma once
#include <charconv>
#include <sstream>

// general implementation from https://stackoverflow.com/a/1243741, with additions
// to_chars casting from https://github.com/apache/arrow/blob/main/cpp/src/arrow/util/string.h

namespace dpp
{
    namespace utility
    {
        class bad_lexical_cast : public std::bad_cast
        {
        public:
            explicit bad_lexical_cast(const char* sourceTypeName, const char* targetTypeName = nullptr);
            explicit bad_lexical_cast(std::string_view message) : message(message) {}
            const char* what() const noexcept override { return message.c_str(); }
        private:
            std::string message;
        };

        struct memstreambuf : std::streambuf
        {
            memstreambuf(const char* base, size_t size)
            {
                char* p(const_cast<char*>(base));
                this->setg(p, p, p + size);
            }
        };

        template<typename T, typename = void>
        struct can_from_chars : std::false_type {};

        template<typename T>
        struct can_from_chars<
            T, std::void_t<decltype(std::from_chars(std::declval<const char*>(), std::declval<const char*>(),
                                                    std::declval<std::add_lvalue_reference_t<T>>()))>>
            : std::true_type {};

        template<typename T>
        inline constexpr bool can_from_chars_v = can_from_chars<T>::value;

        template<typename T, typename = void>
        struct can_to_chars : std::false_type {};

        template<typename T>
        struct can_to_chars<
            T, std::void_t<decltype(std::to_chars(std::declval<char*>(), std::declval<char*>(),
                                                  std::declval<std::remove_reference_t<T>>()))>>
            : std::true_type {};

        template<typename T>
        inline constexpr bool can_to_chars_v = can_to_chars<T>::value;

        namespace casters
        {
            template<typename Target, typename Source>
            struct lexical_caster
            {
                static Target cast(const Source& s)
                {
                    std::stringstream ss;
                    if ((ss << s).fail())
                        throw bad_lexical_cast(typeid(Source).name());

                    Target t;
                    if ((ss >> t).fail() || !(ss >> std::ws).eof())
                        throw bad_lexical_cast(typeid(Source).name(), typeid(Target).name());

                    return t;
                }
            };

            template<typename T>
            struct lexical_caster<T, T>
            {
                static const T& cast(const T& s)
                {
                    return s;
                }
            };

            template<>
            struct lexical_caster<std::string, std::string>
            {
                static const std::string& cast(const std::string& s)
                {
                    return s;
                }
            };

            template<>
            struct lexical_caster<std::string, std::string_view>
            {
                static std::string cast(std::string_view s)
                {
                    return std::string(s);
                }
            };

            template<typename Source>
            struct lexical_caster<std::string, Source>
            {
                static std::string cast(const Source& s)
                {
                    std::ostringstream oss;
                    if ((oss << s).fail())
                        throw bad_lexical_cast(typeid(Source).name());
                    return oss.str();
                }
            };

            template<typename Number> requires std::floating_point<Number> || std::integral<Number>
            struct lexical_caster<std::string, Number>
            {
                static std::string cast(Number n)
                {
                    if constexpr (!can_to_chars_v<Number>)
                        return std::to_string(n);

                    // the "magic numbers" here are to leave room for other characters such as "+-e,."
                    // floating point types need a larger size to account for the decimal part
                    constexpr size_t bufsize = std::integral<Number>
                        ? std::numeric_limits<Number>::digits10 + 2U
                        : std::numeric_limits<Number>::digits10 + std::numeric_limits<Number>::max_digits10 + 10U;

                    char buf[bufsize];
                    const auto res = std::to_chars(buf, buf + bufsize, n);
                    if (res.ec != std::errc())
                        throw bad_lexical_cast(typeid(Number).name(), "std::string");
                    return std::string(buf, res.ptr);
                }
            };

            template<typename Target, typename StringViewLike>
            requires std::convertible_to<const StringViewLike&, std::string_view>
            struct lexical_caster<Target, StringViewLike>
            {
                static Target cast(const StringViewLike& s)
                {
                    if constexpr (can_from_chars_v<Target>)
                    {
                        Target n;
                        if (auto [_, ec] = std::from_chars(s.data(), s.data() + s.size(), n); ec != std::errc())
                            throw bad_lexical_cast(typeid(StringViewLike).name(), typeid(Target).name());
                        return n;
                    }
                    else
                    {
                        memstreambuf sbuf(s.data(), s.size());
                        std::istream in(&sbuf);
                        Target t;
                        if ((in >> t).fail() || !(in >> std::ws).eof())
                            throw bad_lexical_cast(typeid(StringViewLike).name(), typeid(Target).name());
                        return t;
                    }
                }
            };
        }

        template<typename Target, typename Source>
        inline Target lexical_cast(const Source& s, bool exceptions = true)
        {
            if (exceptions)
            {
                return casters::lexical_caster<Target, Source>::cast(s);
            }
            else
            {
                try
                {
                    return casters::lexical_caster<Target, Source>::cast(s);
                }
                catch (const bad_lexical_cast& e)
                {
                    return Target{};
                }
            }
        }
    }
}
