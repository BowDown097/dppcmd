#ifndef LEXICAL_CAST_H
#define LEXICAL_CAST_H
#include <charconv>
#include <sstream>

// general implementation from https://stackoverflow.com/a/1243741, with additions
// to_chars casting from https://github.com/apache/arrow/blob/main/cpp/src/arrow/util/string.h

namespace cmdhndlrutils
{
    class bad_lexical_cast : public std::bad_cast
    {
    public:
        explicit bad_lexical_cast(const char* sourceTypeName, const char* targetTypeName = nullptr);
        explicit bad_lexical_cast(const std::string& message) : message(message) {}
        const char* what() const noexcept override { return message.c_str(); }
    private:
        std::string message;
    };

    namespace casters
    {
        template<typename T, typename = void>
        struct can_from_chars : std::false_type {};

        template<typename T>
        struct can_from_chars<
            T, std::void_t<decltype(std::from_chars(std::declval<const char*>(), std::declval<const char*>(),
                                                    std::declval<std::add_lvalue_reference_t<T>>()))>>
            : std::true_type {};

        template<typename T, typename = void>
        struct can_to_chars : std::false_type {};

        template <typename T>
        struct can_to_chars<
            T, std::void_t<decltype(std::to_chars(std::declval<char*>(), std::declval<char*>(),
                                                  std::declval<std::remove_reference_t<T>>()))>>
            : std::true_type {};

        template<typename T>
        inline constexpr bool has_from_chars = can_from_chars<T>::value;

        template<typename T>
        inline constexpr bool has_to_chars = can_to_chars<T>::value;

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

        template<typename Target>
        struct lexical_caster<Target, std::string>
        {
            static Target cast(const std::string& s)
            {
                std::istringstream ss(s);
                Target t;
                if ((ss >> t).fail() || !(ss >> std::ws).eof())
                    throw bad_lexical_cast("std::string", typeid(Target).name());
                return t;
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

        template<typename Number> requires has_from_chars<Number>
        struct lexical_caster<Number, std::string>
        {
            static Number cast(const std::string& s)
            {
                Number n;
                if (auto [_, ec] = std::from_chars(s.data(), s.data() + s.size(), n); ec != std::errc())
                    throw bad_lexical_cast("std::string", typeid(Number).name());
                return n;
            }
        };

        template<typename Number> requires std::floating_point<Number> || std::integral<Number>
        struct lexical_caster<std::string, Number>
        {
            static std::string cast(Number n)
            {
                if constexpr (!has_to_chars<Number>)
                    return std::to_string(n);

                std::string out(15, 0);

                auto res = std::to_chars(&out.front(), &out.back(), n);
                while (res.ec != std::errc())
                {
                    if (res.ec != std::errc::value_too_large)
                        throw bad_lexical_cast(typeid(Number).name(), "std::string");
                    out.resize(out.capacity() * 2);
                    res = std::to_chars(&out.front(), &out.back(), n);
                }

                const auto length = res.ptr - out.data();
                if (length > static_cast<int64_t>(out.length()))
                    throw bad_lexical_cast(typeid(Number).name(), "std::string");

                out.resize(length);
                return out;
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
            catch (const bad_lexical_cast& e) { return Target{}; }
        }
    }
}

#endif // LEXICAL_CAST_H
