cmake_minimum_required(VERSION 3.10)
project(msg-queue-SYSV)

set(CMAKE_C_STANDARD 11)

add_executable(msg-queue
        src/mq_main.c
        include/mq_main.h)
include_directories(include)
