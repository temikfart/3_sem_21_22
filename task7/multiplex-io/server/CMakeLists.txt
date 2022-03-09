CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(multiplex-io)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pthread")
set(EXECUTABLE_NAME "server")

include_directories(include)
add_executable(${EXECUTABLE_NAME}
        src/thpool.c
        include/thpool.h
        src/s_interact.c
        include/s_interact.h
        src/s_main.c
        include/s_main.h
        )
