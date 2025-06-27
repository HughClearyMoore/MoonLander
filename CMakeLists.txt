cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(MoonLander)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/downloads")
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    STI
    GIT_REPOSITORY https://github.com/HughClearyMoore/STI.git
    GIT_TAG main
)

FetchContent_MakeAvailable(STI)

add_executable(MoonLander "${CMAKE_SOURCE_DIR}/src/MoonLander.c")

add_subdirectory(src)

target_link_libraries(MoonLander PRIVATE STI)

