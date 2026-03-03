#pragma once

#include "Mesh.h"
#include <vector>

// Scene manages all game objects/meshes.
// Future: Add hierarchy, transforms, culling, etc.

class Scene {
public:
    Scene() = default;
    ~Scene() {
        // Clean up owned meshes
        for (auto* mesh : m_meshes) {
            delete mesh;
        }
    }

    // Add a mesh to the scene (scene takes ownership)
    void AddMesh(Mesh* mesh) {
        m_meshes.push_back(mesh);
    }

    // Get all meshes for rendering
    const std::vector<Mesh*>& GetMeshes() const {
        return m_meshes;
    }

private:
    std::vector<Mesh*> m_meshes;
};
