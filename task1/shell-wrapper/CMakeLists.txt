cmake_minimum_required(VERSION 3.10)
project(shell-wrapper)

set(CMAKE_C_STANDARD 11)

add_executable(shell-wrapper src/sw_main.c src/sw_parse.c include/sw_parse.h)
include_directories(include)
