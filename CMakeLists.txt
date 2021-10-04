# This file is only here for CLion to work, since it cannot handle Make projects
cmake_minimum_required(VERSION 3.20)

project(mini_os C)

set(CMAKE_C_COMPILER x86_64-elf-gcc)

add_executable(kernel src/kernel/src/Kernel.c)
add_executable(os src/os/src/Main.c)

target_include_directories(kernel BEFORE PRIVATE src/kernel/include)
target_include_directories(os BEFORE PRIVATE src/os/include)
target_include_directories(os BEFORE PRIVATE ports/musl-1.2.2/include)
target_include_directories(os BEFORE PRIVATE ports/musl-1.2.2/obj/include)
target_include_directories(os BEFORE PRIVATE ports/musl-1.2.2/arch/x86_64)
target_include_directories(os BEFORE PRIVATE ports/musl-1.2.2/arch/generic)
target_include_directories(os BEFORE PRIVATE ports/doomgeneric/doomgeneric)
