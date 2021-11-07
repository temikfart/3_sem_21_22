cmake_minimum_required(VERSION 3.10)
project(sig-transf)

set(CMAKE_C_STANDARD 11)

include_directories(include)
add_executable(sig-transf
        src/st_main.c
        include/st_main.h)
