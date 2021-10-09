cmake_minimum_required(VERSION 3.10)
project(FIFO)

set(CMAKE_C_STANDARD 11)

add_executable(fifo
        src/fifo_main.c
        include/fifo_main.h)
include_directories(include)
