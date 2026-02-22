# Step 1: Open a Window and Clear It to a Color

This is the engine's "Hello World." Before any 3D, physics, or assets — you need a window
with a working context. In this case we are using OpenGL. That's it. That's the whole step. In future steps we will use DirectX and Vulkan.

**What you'll use:**
- **GLFW** — a small C library that opens a window and creates an OpenGL context for you.
  It handles the ugly Win32/WGL boilerplate so you can focus on the engine, not the OS.
- **OpenGL** — the graphics API that will draw into that window.
- **GLAD** — a small loader that gives you access to modern OpenGL functions at runtime.
  OpenGL on Windows ships as an ancient stub (GL 1.1) — GLAD fetches the real function pointers.

**Install GLFW and GLAD via MSYS2:**
```
pacman -S mingw-w64-ucrt-x86_64-glfw
```
GLAD is a generated single-header — download it from https://glad.dav1d.de:
  - Language: C/C++
  - Specification: OpenGL
  - Profile: Core
  - Version: 4.6
  - Click "Generate", download the zip, copy `glad.c` and `include/` into your project.

**Project structure:**
```
GameEngine/
  src/
    main.cpp
    glad.c
  include/
    glad/glad.h
    KHR/khrplatform.h
  CMakeLists.txt
```

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(engine)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)

add_executable(engine src/main.cpp src/glad.c)
target_include_directories(engine PRIVATE include)
target_link_libraries(engine glfw opengl32)
```

**main.cpp — the entire first step:**
```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
```

**Build and run:**
```
cmake -B build -G Ninja
cmake --build build
./build/engine.exe
```

**What success looks like:**
A dark grey window opens. ESC closes it. Nothing else. That's the foundation every
3D engine is built on — a window, a context, and a loop.
