cmake_minimum_required(VERSION 3.10)
project(FIFO)

set(CMAKE_C_STANDARD 11)

include_directories(include)
add_executable(fifo-server
        src/fifo_server.c
        include/fifo_server.h)

include_directories(include)
add_executable(fifo-client
        src/fifo_client.c
        include/fifo_client.h)

