#pragma once

#include "renderer/IRenderer.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer()  = default;
    ~VulkanRenderer() override = default;

    bool Init(void* windowHandle) override;
    void BeginFrame() override;
    void DrawMesh(const Mesh* mesh) override;
    void EndFrame()   override;
    void Shutdown()   override;

private:
    // Core
    VkInstance               m_instance       = VK_NULL_HANDLE;
    VkSurfaceKHR             m_surface        = VK_NULL_HANDLE;
    VkPhysicalDevice         m_physicalDevice = VK_NULL_HANDLE;
    VkDevice                 m_device         = VK_NULL_HANDLE;
    VkQueue                  m_graphicsQueue  = VK_NULL_HANDLE;
    uint32_t                 m_queueFamily    = 0;

    // Swap chain
    VkSwapchainKHR             m_swapchain       = VK_NULL_HANDLE;
    VkFormat                   m_swapchainFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D                 m_swapchainExtent = {};
    std::vector<VkImage>       m_swapchainImages;
    std::vector<VkImageView>   m_swapchainImageViews;

    // Render pass & pipeline
    VkRenderPass               m_renderPass     = VK_NULL_HANDLE;
    VkPipelineLayout           m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline                 m_pipeline       = VK_NULL_HANDLE;

    // Framebuffers
    std::vector<VkFramebuffer> m_framebuffers;

    // Commands
    VkCommandPool              m_commandPool   = VK_NULL_HANDLE;
    VkCommandBuffer            m_commandBuffer = VK_NULL_HANDLE;

    // Vertex buffer (host-visible, up to 1024 vertices)
    VkBuffer                   m_vertexBuffer       = VK_NULL_HANDLE;
    VkDeviceMemory             m_vertexBufferMemory = VK_NULL_HANDLE;

    // Synchronisation
    VkSemaphore                m_imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore                m_renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence                    m_inFlightFence           = VK_NULL_HANDLE;

    // Per-frame state
    uint32_t                   m_currentImageIndex = 0;
    uint32_t                   m_width  = 1280;
    uint32_t                   m_height = 720;

    // Helpers
    uint32_t       FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props) const;
    VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirv) const;
};
