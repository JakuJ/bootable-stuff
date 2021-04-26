cmake_minimum_required(VERSION 3.19)
project(mini_os C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER gcc-10)

add_executable(mini_os main.c)