cmake_minimum_required(VERSION 3.10)
project(integrate-with-mk)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

include_directories(include)
add_executable(integrate
        src/mk_main.c
        include/mk_main.h)
