cmake_minimum_required(VERSION 3.20)
project(Snake CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Cupcake is an installed package — no knowledge of its source tree required
find_package(Cupcake REQUIRED)

add_executable(Snake
    src/main.cpp
)

target_link_libraries(Snake
    PRIVATE Cupcake::Cupcake
)
