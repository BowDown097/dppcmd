project(example-bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example-bot
    main.cpp
    credentials.h credentials.cpp
    epicenum.h epicenum.cpp
    testmodule.h testmodule.cpp)

target_link_libraries(example-bot PRIVATE dppcmd)
