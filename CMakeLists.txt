add_library(dpp-command-handler)

set_target_properties(dpp-command-handler
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)

option(USE_COROUTINES "Use DPP coroutine support." OFF)

if(USE_COROUTINES)
    target_compile_definitions(dpp-command-handler PUBLIC DPP_CORO)
endif()

target_sources(dpp-command-handler
    PRIVATE
        dpp-command-handler/commandparser.cpp
        dpp-command-handler/module.cpp
        dpp-command-handler/moduleservice.cpp
        dpp-command-handler/readers/channeltypereader.cpp
        dpp-command-handler/readers/roletypereader.cpp
        dpp-command-handler/readers/usertypereader.cpp
        dpp-command-handler/utils/lexical_cast.cpp
        dpp-command-handler/utils/mentions.cpp
        dpp-command-handler/utils/strings.cpp
    PUBLIC
        FILE_SET HEADERS FILES
            dpp-command-handler/commandinfo.h
            dpp-command-handler/commandparser.h
            dpp-command-handler/module.h
            dpp-command-handler/moduleservice.h
            dpp-command-handler/precondition.h
            dpp-command-handler/variantfunction.h
            dpp-command-handler/readers/channeltypereader.h
            dpp-command-handler/readers/roletypereader.h
            dpp-command-handler/readers/typereader.h
            dpp-command-handler/readers/usertypereader.h
            dpp-command-handler/results/commanderror.h
            dpp-command-handler/results/commandresult.h
            dpp-command-handler/results/preconditionresult.h
            dpp-command-handler/results/result.h
            dpp-command-handler/results/typereaderresult.h
            dpp-command-handler/utils/ezcoro.h
            dpp-command-handler/utils/join.h
            dpp-command-handler/utils/lexical_cast.h
            dpp-command-handler/utils/mentions.h
            dpp-command-handler/utils/strings.h
            dpp-command-handler/utils/traits.h)

find_package(DPP QUIET)
if(DPP_FOUND)
    target_link_libraries(dpp-command-handler PUBLIC dpp::dpp)
else()
    # get version 10.0.24 (latest as of 7/1/2023)
    include(FetchContent)
    option(BUILD_VOICE_SUPPORT "" OFF)
    option(DPP_BUILD_TEST "" OFF)
    FetchContent_Declare(libdpp GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git GIT_TAG 63ac3fed8bdd92e3122c6079b944f70fc9546dc2)

    FetchContent_GetProperties(libdpp)
    if(NOT libdpp_POPULATED)
        FetchContent_Populate(libdpp)
        target_include_directories(dpp-command-handler PUBLIC ${libdpp_SOURCE_DIR}/include)
        add_subdirectory(${libdpp_SOURCE_DIR} ${libdpp_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    target_link_libraries(dpp-command-handler PUBLIC dpp)
endif()
