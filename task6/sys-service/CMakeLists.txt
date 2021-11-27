CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(system-service)

set(CMAKE_C_STANDARD 11)
set(COMPILE_FLAGS "-Wall -Wextra")
set(EXECUTABLE_NAME "service")

include_directories(include)
add_executable(${EXECUTABLE_NAME}
        src/ss_main.c
        include/ss_main.h)

target_compile_options(${EXECUTABLE_NAME} PUBLIC "SHELL: ${COMPILE_FLAGS}")
