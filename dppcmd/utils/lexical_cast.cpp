#include "lexical_cast.h"
#include "strings.h"

namespace dppcmd
{
    namespace utility
    {
        bad_lexical_cast::bad_lexical_cast(const char* sourceTypeName, const char* targetTypeName)
            : message(targetTypeName == nullptr
                          ? "Failed to convert from " + demangle(sourceTypeName)
                          : "Failed to convert from " + demangle(sourceTypeName) +
                            " to " + demangle(targetTypeName)) {}
    }
}
