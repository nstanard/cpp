#pragma once

#include <cstdint>
#include <vector>

// Generic vertex structure used by all meshes
struct Vertex {
    float position[3];
    float color[4];
};

// Mesh is a plain data container — game code supplies the vertex array.
// The engine never hard-codes shapes here; those live in game/application code.
class Mesh {
public:
    Mesh(const Vertex* vertices, uint32_t count)
        : m_vertices(vertices, vertices + count) {}

    const Vertex* GetVertices()  const { return m_vertices.data(); }
    uint32_t      GetVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }

private:
    std::vector<Vertex> m_vertices;
};
