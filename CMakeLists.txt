add_library(dpp-command-handler)

set_target_properties(dpp-command-handler
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)

option(USE_COROUTINES "Use DPP coroutine support." OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(USE_COROUTINES)
    target_compile_definitions(dpp-command-handler PUBLIC DPP_CORO)
endif()

target_sources(dpp-command-handler
    PRIVATE
        dpp-command-handler/commands/commandinfo.cpp
        dpp-command-handler/commands/commandparser.cpp
        dpp-command-handler/commands/exceptions.cpp
        dpp-command-handler/extensions/cache.cpp
        dpp-command-handler/modules/modulebase.cpp
        dpp-command-handler/readers/channeltypereader.cpp
        dpp-command-handler/readers/guildmembertypereader.cpp
        dpp-command-handler/readers/roletypereader.cpp
        dpp-command-handler/readers/usertypereader.cpp
        dpp-command-handler/services/basecommandservice.cpp
        dpp-command-handler/services/commandservice.cpp
        dpp-command-handler/services/moduleservice.cpp
        dpp-command-handler/utils/lexical_cast.cpp
        dpp-command-handler/utils/mentions.cpp
        dpp-command-handler/utils/strings.cpp
    PUBLIC
        FILE_SET HEADERS FILES
            dpp-command-handler/commands/commandexecution.h
            dpp-command-handler/commands/commandfunction.h
            dpp-command-handler/commands/commandinfo.h
            dpp-command-handler/commands/commandparser.h
            dpp-command-handler/commands/exceptions.h
            dpp-command-handler/commands/precondition.h
            dpp-command-handler/commands/remainder.h
            dpp-command-handler/extensions/cache.h
            dpp-command-handler/modules/module.h
            dpp-command-handler/modules/modulebase.h
            dpp-command-handler/readers/channeltypereader.h
            dpp-command-handler/readers/guildmembertypereader.h
            dpp-command-handler/readers/roletypereader.h
            dpp-command-handler/readers/typereader.h
            dpp-command-handler/readers/usertypereader.h
            dpp-command-handler/results/commanderror.h
            dpp-command-handler/results/commandresult.h
            dpp-command-handler/results/preconditionresult.h
            dpp-command-handler/results/result.h
            dpp-command-handler/results/typereaderresult.h
            dpp-command-handler/services/basecommandservice.h
            dpp-command-handler/services/commandservice.h
            dpp-command-handler/services/moduleservice.h
            dpp-command-handler/utils/ezcoro.h
            dpp-command-handler/utils/function_traits.h
            dpp-command-handler/utils/join.h
            dpp-command-handler/utils/lexical_cast.h
            dpp-command-handler/utils/mentions.h
            dpp-command-handler/utils/strings.h
            dpp-command-handler/utils/tuple_traits.h
            dpp-command-handler/utils/type_traits.h)

find_package(DPP REQUIRED)
target_link_libraries(dpp-command-handler PUBLIC dpp::dpp)
