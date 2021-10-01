cmake_minimum_required(VERSION 3.10)
project(duplex-pipe)

set(CMAKE_C_STANDARD 11)

add_executable(duplex-pipe
        src/dp_main.c
        src/dp_string.c
        src/dp_pipe.c
        include/dp_main.h
        include/dp_string.h
        include/dp_pipe.h)
include_directories(include)
