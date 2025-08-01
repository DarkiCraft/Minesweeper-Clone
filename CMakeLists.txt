cmake_minimum_required(VERSION 3.11)
project(Minesweeper C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(minesweeper main.c)

include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.0
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

target_link_libraries(minesweeper raylib)

if (WIN32)
    target_link_libraries(minesweeper winmm gdi32)
elseif(APPLE)
    target_link_libraries(minesweeper
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()
