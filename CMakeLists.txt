cmake_minimum_required(VERSION 3.20)

project(mini_os C)

include_directories(src/kernel/include)
include_directories(src/libc/include)

add_executable(kernel src/kernel/src/Kernel.c)
