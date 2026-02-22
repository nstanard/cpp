#pragma once

// IRenderer — the abstract graphics device interface.
//
// All engine systems talk to this interface only. No engine code outside of
// RendererFactory.cpp ever names a concrete backend type.

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool Init()       = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame()   = 0;
    virtual void Shutdown()   = 0;
};

// Factory function — defined in RendererFactory.cpp.
// Returns a heap-allocated backend selected at compile time.
// The caller owns the pointer and must call Shutdown() then delete it.
IRenderer* CreateRenderer();
