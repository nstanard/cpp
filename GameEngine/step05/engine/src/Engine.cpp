#include "Engine.h"
#include "renderer/IRenderer.h"
#include <iostream>

Engine::Engine() = default;

Engine::~Engine() {
    if (m_renderer) {
        delete m_renderer;
    }
}

bool Engine::Init(void* nativeWindowHandle) {
    std::cout << "[Engine] Initializing...\n";

    m_renderer = CreateRenderer();
    if (!m_renderer) {
        std::cerr << "[Engine] Failed to create renderer\n";
        return false;
    }

    if (!m_renderer->Init(nativeWindowHandle)) {
        std::cerr << "[Engine] Renderer initialization failed\n";
        return false;
    }

    std::cout << "[Engine] Initialization complete\n";
    return true;
}

void Engine::Update() {
    m_renderer->BeginFrame();

    for (const auto* mesh : m_scene.GetMeshes()) {
        m_renderer->DrawMesh(mesh);
    }

    m_renderer->EndFrame();
}

void Engine::Shutdown() {
    std::cout << "[Engine] Shutting down...\n";

    if (m_renderer) {
        m_renderer->Shutdown();
    }

    std::cout << "[Engine] Shutdown complete\n";
}
