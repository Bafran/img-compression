cmake_minimum_required(VERSION 3.21)
project(compression_v2 C)

set(CMAKE_C_STANDARD 99)

add_executable(compression_v2 main.c compression.c compression.h image.c image.h utils.h huff.c huff.h tables.c tables.h config.h utils.c)
