cmake_minimum_required(VERSION 3.12)
project(Minesweeper C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(minesweeper main.c)

find_package(raylib REQUIRED)

target_link_libraries(minesweeper raylib winmm gdi32)