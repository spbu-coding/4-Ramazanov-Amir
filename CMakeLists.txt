cmake_minimum_required(VERSION 3.7)
project(tests LANGUAGES C)

set(CMAKE_C_STANDARD 99)

add_executable(converter converter.c bmp_info.c bmp_info.h)
add_executable(comparer comparer.c bmp_info.c bmp_info.h qdbmp.c qdbmp.h)

