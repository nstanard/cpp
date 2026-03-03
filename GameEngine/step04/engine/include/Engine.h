#pragma once

#include "Scene.h"
#include <GLFW/glfw3.h>

class IRenderer;  // defined in src/renderer/IRenderer.h — internal detail

// Engine owns the renderer and coordinates the game loop.
// Game code interacts with the Engine, not the renderer directly.

class Engine {
public:
    Engine();
    ~Engine();

    // Initialize engine with a window
    bool Init(GLFWwindow* window);
    
    // Get the scene to add/remove objects
    Scene* GetScene() { return &m_scene; }
    
    // Update and render one frame
    void Update();
    
    // Shutdown engine
    void Shutdown();

private:
    IRenderer* m_renderer = nullptr;
    Scene      m_scene;
};
