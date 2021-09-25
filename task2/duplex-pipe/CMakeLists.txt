cmake_minimum_required(VERSION 3.10)
project(duplex-pipe)

set(CMAKE_C_STANDARD 11)

add_executable(duplex-pipe
        src/dp_main.c
        include/dp_main.h)
include_directories(include)
