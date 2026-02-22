cmake_minimum_required(VERSION 3.20)
project(engine)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)

add_executable(engine src/main.cpp src/glad.c)
target_include_directories(engine PRIVATE include)
target_link_libraries(engine glfw opengl32)
