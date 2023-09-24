cmake_minimum_required(VERSION 3.10)
project(ColonelBlotto)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_GENERATOR "MinGW Makefiles")
message("generator is set to ${CMAKE_GENERATOR}")

set(CMAKE_C_STANDARD "99")
set(CMAKE_C_COMPILER "C:\\msys64\\mingw64\\bin\\gcc.exe")
set(CMAKE_BUILD_TYPE "Debug")

file(GLOB all_SRCS
        "include/*.h"
        "src/*.c"
        "main.c"
        )

message("files are ${all_SRCS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
add_executable(main ${all_SRCS})