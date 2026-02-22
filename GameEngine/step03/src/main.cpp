#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer/IRenderer.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return 1;
    }

    // No OpenGL context — DX12 and Vulkan create their own swap chains.
    // GLFW here is purely a window manager that provides a native HWND.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Engine v0", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    IRenderer* renderer = CreateRenderer();

    // Stubs don't use the window handle yet.
    // A real backend will receive the HWND / VkSurfaceKHR here.
    if (!renderer->Init()) {
        std::cerr << "Renderer Init() failed\n";
        delete renderer;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        renderer->BeginFrame();
        
        // TODO: Render a triangle

        renderer->EndFrame();

        glfwPollEvents();
    }

    renderer->Shutdown();
    delete renderer;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
