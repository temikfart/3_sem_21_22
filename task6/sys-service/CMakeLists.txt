CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(system-service)

set(CMAKE_C_STANDARD 11)
set(COMPILE_FLAGS "-Wall -Wextra")
set(EXECUTABLE_NAME "service")

include_directories(include)
add_executable(${EXECUTABLE_NAME}
        src/ss_main.c
        src/ss_log.c
        src/ss_start.c
        src/ss_config.c
        src/ss_error.c
        include/ss_log.h
        include/ss_start.h
        include/ss_main.h
        include/ss_config.h
        include/ss_error.h
        )

target_compile_options(${EXECUTABLE_NAME} PUBLIC "SHELL: ${COMPILE_FLAGS}")
