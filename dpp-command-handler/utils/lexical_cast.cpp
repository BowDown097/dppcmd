#include "lexical_cast.h"

#if defined(__has_include) && __has_include(<cxxabi.h>) && !defined(__GABIXX_CXXABI_H__)
# include <cxxabi.h>
# include <memory>
# define NEED_DEMANGLE
#endif

std::string demangle(const char* name)
{
#ifdef NEED_DEMANGLE
    int status = -1;
    std::unique_ptr<char, decltype(&free)> demangled(abi::__cxa_demangle(name, nullptr, nullptr, &status), free);
    return status == 0 ? demangled.get() : name;
#else
    return name;
#endif
}

namespace dpp
{
    namespace utility
    {
        bad_lexical_cast::bad_lexical_cast(const char* sourceTypeName, const char* targetTypeName)
            : message(targetTypeName == nullptr
                          ? "Failed to stream from source type " + demangle(sourceTypeName)
                          : "Failed to stream from source type " + demangle(sourceTypeName) +
                            " to target type " + demangle(targetTypeName)) {}
    }
}
