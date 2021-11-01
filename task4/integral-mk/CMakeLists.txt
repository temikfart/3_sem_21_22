cmake_minimum_required(VERSION 3.10)
project(integrate-with-mk)

set(CMAKE_C_STANDARD 11)

include_directories(include)
add_executable(integrate
        src/mk_main.c
        include/mk_main.h)
