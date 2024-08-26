#include "lexical_cast.h"
#include "strings.h"

namespace dppcmd
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
