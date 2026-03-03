#include "renderer/IRenderer.h"

// The single compile-time seam. Exactly one branch compiles — the other
// backend's header is never included or parsed in this translation unit.

#ifdef RENDERER_DX12
    #include "renderer/dx12/DX12Renderer.h"
#elif defined(RENDERER_VULKAN)
    #include "renderer/vulkan/VulkanRenderer.h"
#else
    #error "No renderer backend defined. Pass -DRENDERER=DX12 or -DRENDERER=VULKAN to cmake."
#endif

IRenderer* CreateRenderer() {
#ifdef RENDERER_DX12
    return new DX12Renderer();
#elif defined(RENDERER_VULKAN)
    return new VulkanRenderer();
#endif
}
