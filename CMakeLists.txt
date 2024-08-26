add_library(dppcmd)

set_target_properties(dppcmd
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)

option(USE_COROUTINES "Use DPP coroutine support." OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(USE_COROUTINES)
    target_compile_definitions(dppcmd PUBLIC DPP_CORO)
endif()

target_sources(dppcmd
    PRIVATE
	dppcmd/commands/commandinfo.cpp
	dppcmd/commands/commandparser.cpp
	dppcmd/commands/exceptions.cpp
	dppcmd/extensions/cache.cpp
	dppcmd/modules/modulebase.cpp
	dppcmd/readers/channeltypereader.cpp
	dppcmd/readers/guildmembertypereader.cpp
	dppcmd/readers/roletypereader.cpp
	dppcmd/readers/usertypereader.cpp
	dppcmd/services/basecommandservice.cpp
	dppcmd/services/commandservice.cpp
	dppcmd/services/moduleservice.cpp
	dppcmd/utils/lexical_cast.cpp
	dppcmd/utils/mentions.cpp
	dppcmd/utils/strings.cpp
    PUBLIC
        FILE_SET HEADERS FILES
	    dppcmd/commands/commandexecution.h
	    dppcmd/commands/commandfunction.h
	    dppcmd/commands/commandinfo.h
	    dppcmd/commands/commandparser.h
	    dppcmd/commands/exceptions.h
	    dppcmd/commands/precondition.h
	    dppcmd/commands/remainder.h
	    dppcmd/extensions/cache.h
	    dppcmd/modules/module.h
	    dppcmd/modules/modulebase.h
	    dppcmd/readers/channeltypereader.h
	    dppcmd/readers/guildmembertypereader.h
	    dppcmd/readers/roletypereader.h
	    dppcmd/readers/typereader.h
	    dppcmd/readers/usertypereader.h
	    dppcmd/results/commanderror.h
	    dppcmd/results/commandresult.h
	    dppcmd/results/preconditionresult.h
	    dppcmd/results/result.h
	    dppcmd/results/typereaderresult.h
	    dppcmd/services/basecommandservice.h
	    dppcmd/services/commandservice.h
	    dppcmd/services/moduleservice.h
	    dppcmd/utils/ezcoro.h
	    dppcmd/utils/function_traits.h
	    dppcmd/utils/join.h
	    dppcmd/utils/lexical_cast.h
	    dppcmd/utils/mentions.h
	    dppcmd/utils/strings.h
	    dppcmd/utils/tuple_traits.h
	    dppcmd/utils/type_traits.h)

find_package(DPP REQUIRED)
target_link_libraries(dppcmd PUBLIC dpp::dpp)
