#ifndef LEXICAL_CAST_H
#define LEXICAL_CAST_H
#include <charconv>
#include <sstream>

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

    // https://stackoverflow.com/a/1243741, with additions
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

        // std::from/to_chars is only used for integrals as floating point
        // support for them has STILL not been implemented in some compilers.

        template<std::integral Integral>
        struct lexical_caster<Integral, std::string>
        {
            static Integral cast(const std::string& s)
            {
                Integral i;
                if (auto [_, ec] = std::from_chars(s.data(), s.data() + s.size(), i); ec != std::errc())
                    throw bad_lexical_cast("std::string", typeid(Integral).name());
                return i;
            }
        };

        // https://github.com/apache/arrow/blob/main/cpp/src/arrow/util/string.h
        template<std::integral Integral>
        struct lexical_caster<std::string, Integral>
        {
            static std::string cast(Integral i)
            {
                std::string out(15, 0);

                auto res = std::to_chars(&out.front(), &out.back(), i);
                while (res.ec != std::errc())
                {
                    if (res.ec != std::errc::value_too_large)
                        throw bad_lexical_cast(typeid(Integral).name(), "std::string");
                    out.resize(out.capacity() * 2);
                    res = std::to_chars(&out.front(), &out.back(), i);
                }

                const auto length = res.ptr - out.data();
                if (length > static_cast<int64_t>(out.length()))
                    throw bad_lexical_cast(typeid(Integral).name(), "std::string");

                out.resize(length);
                return out;
            }
        };

        template<std::floating_point FloatingPoint>
        struct lexical_caster<std::string, FloatingPoint>
        {
            static std::string cast(FloatingPoint f)
            {
                return std::to_string(f);
            }
        };
    }

    template<typename Target, typename Source>
    inline Target lexical_cast(const Source& s)
    {
        return casters::lexical_caster<Target, Source>::cast(s);
    }
}

#endif // LEXICAL_CAST_H
