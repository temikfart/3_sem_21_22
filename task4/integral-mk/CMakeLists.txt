cmake_minimum_required(VERSION 3.10)
project(integrate-with-mk)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-pthread -O0")
add_definitions("-pthread")

include_directories(include)
add_executable(integrate
        src/mk_main.c
        include/mk_main.h)

target_link_libraries(integrate m)
