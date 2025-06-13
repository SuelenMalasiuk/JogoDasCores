cmake_minimum_required(VERSION 3.10)
project(JogoDasCores)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
include_directories(src)

link_directories(lib)

add_executable(JogoDasCores
    src/main.cpp
    src/glad.c
)

target_link_libraries(JogoDasCores
    glfw3
    opengl32
)
