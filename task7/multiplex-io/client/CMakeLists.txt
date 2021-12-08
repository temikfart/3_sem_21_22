CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(multiplex-io)

set(CMAKE_C_STANDARD 11)
set(COMPILE_FLAGS "-Wall -Wextra")
set(EXECUTABLE_NAME "client")

include_directories(include)
add_executable(${EXECUTABLE_NAME}
        src/cn_main.c
        include/cn_main.h
        )

target_compile_options(${EXECUTABLE_NAME} PUBLIC "SHELL: ${COMPILE_FLAGS}")
