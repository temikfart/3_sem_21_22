cmake_minimum_required(VERSION 3.10)
project(FIFO)

set(CMAKE_C_STANDARD 11)

add_executable(fifo-server
        src/fifo_server.c
        include/fifo_main.h)
add_executable(fifo-client
        src/fifo_client.c
        include/fifo_main.h)

include_directories(include)
