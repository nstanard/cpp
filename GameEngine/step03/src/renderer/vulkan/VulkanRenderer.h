#pragma once

// Only compiled when RENDERER_VULKAN is defined.
// No Vulkan headers included here — stubs prove the architecture before real GPU work.

#include "renderer/IRenderer.h"

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer()  = default;
    ~VulkanRenderer() override = default;

    bool Init()       override;
    void BeginFrame() override;
    void EndFrame()   override;
    void Shutdown()   override;
};
