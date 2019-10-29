# cmake_minimum_required(VERSION <specify CMake version here>)
project(trident C)

set(CMAKE_C_STANDARD 99)

add_executable(trident main.c slog.c)
