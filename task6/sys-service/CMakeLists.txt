PROJECT(system-service)

set(CMAKE_C_STANDARD 11)

include_directories(include)
add_executable(service
        src/ss_main.c
        include/ss_main.h)
