#include <Engine.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "[Snake] Failed to initialize GLFW\n";
        return 1;
    }

    // Vulkan needs no OpenGL context; DX12 needs no context either
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Snake", nullptr, nullptr);
    if (!window) {
        std::cerr << "[Snake] Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    Engine engine;
    if (!engine.Init(window)) {
        std::cerr << "[Snake] Engine init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Placeholder triangle so the renderer has something to draw
    Vertex triangle[] = {
        {{ 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };
    engine.GetScene()->AddMesh(new Mesh(triangle, 3));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        engine.Update();
    }

    engine.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
