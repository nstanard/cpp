#pragma once

// Only compiled when RENDERER_DX12 is defined.
// No DX12 headers included here — stubs prove the architecture before real GPU work.

#include "renderer/IRenderer.h"

class DX12Renderer : public IRenderer {
public:
    DX12Renderer()  = default;
    ~DX12Renderer() override = default;

    bool Init()       override;
    void BeginFrame() override;
    void EndFrame()   override;
    void Shutdown()   override;
};
