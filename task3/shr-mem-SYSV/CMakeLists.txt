cmake_minimum_required(VERSION 3.10)
project(shr-mem-SYSV)

set(CMAKE_C_STANDARD 11)

add_executable(shr-mem
        src/sm_main.c
        include/sm_main.h)
include_directories(include)
