cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(MoonLander)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/downloads")
set(FETCHCONTENT_QUIET OFF)

set(GLFW_INSTALL OFF)

set(ODE_WITH_TESTS OFF)
set(ODE_WITH_DEMOS OFF)
set(STI_BUILD_TESTS OFF)



FetchContent_Declare(
    STI
    GIT_REPOSITORY https://github.com/HughClearyMoore/STI.git
    GIT_TAG main
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG e86f90457371c6233053bacf0d6f486a51ddcd67
)

FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG 144d1e7c29b3b0c6dede7917a0476cc95248559c
)

FetchContent_Declare(
    ode
    GIT_REPOSITORY https://bitbucket.org/odedevs/ode.git
    GIT_TAG "master"
)

FetchContent_MakeAvailable(STI glfw glad cglm ode)

find_package(OpenGL REQUIRED)

add_executable(MoonLander "${CMAKE_SOURCE_DIR}/src/MoonLander.c")

add_subdirectory(src)

target_link_libraries(MoonLander PRIVATE STI glfw glad cglm OpenGL::GL ODE)




