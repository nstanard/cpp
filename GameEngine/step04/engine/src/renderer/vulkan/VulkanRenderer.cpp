#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "renderer/vulkan/VulkanRenderer.h"
#include "Mesh.h"

#include <shaderc/shaderc.hpp>
#include <iostream>
#include <cstring>
#include <vector>

// ─── Inline shader source (mirrors DX12's inline HLSL) ───────────────────────

static const char* kVertGLSL = R"(
#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 0) out vec4 fragColor;
void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColor   = inColor;
}
)";

static const char* kFragGLSL = R"(
#version 450
layout(location = 0) in  vec4 fragColor;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = fragColor;
}
)";

// ─── Module-local helpers ─────────────────────────────────────────────────────

static std::vector<uint32_t> CompileGLSL(
    const char* source, shaderc_shader_kind kind, const char* label)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    auto result = compiler.CompileGlslToSpv(source, kind, label, options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << "[Vulkan] Shader error (" << label << "): "
                  << result.GetErrorMessage() << "\n";
        return {};
    }
    return { result.cbegin(), result.cend() };
}

// ─── Init ─────────────────────────────────────────────────────────────────────

bool VulkanRenderer::Init(void* windowHandle) {
    std::cout << "[Vulkan] Initializing...\n";
    GLFWwindow* window = static_cast<GLFWwindow*>(windowHandle);

    // Instance
    VkApplicationInfo appInfo{};
    appInfo.sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t    glfwExtCount = 0;
    const char** glfwExts   = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    VkInstanceCreateInfo instanceCI{};
    instanceCI.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pApplicationInfo        = &appInfo;
    instanceCI.enabledExtensionCount   = glfwExtCount;
    instanceCI.ppEnabledExtensionNames = glfwExts;

#ifdef _DEBUG
    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
    instanceCI.enabledLayerCount   = 1;
    instanceCI.ppEnabledLayerNames = layers;
#endif

    if (vkCreateInstance(&instanceCI, nullptr, &m_instance) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create instance\n";
        return false;
    }

    // Surface
    if (glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create window surface\n";
        return false;
    }

    // Physical device — pick first that has a graphics+present queue
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> physDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, physDevices.data());

    for (auto pd : physDevices) {
        uint32_t qCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &qCount, nullptr);
        std::vector<VkQueueFamilyProperties> qFamilies(qCount);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &qCount, qFamilies.data());

        for (uint32_t i = 0; i < qCount; i++) {
            VkBool32 canPresent = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(pd, i, m_surface, &canPresent);
            if ((qFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && canPresent) {
                m_physicalDevice = pd;
                m_queueFamily    = i;
                break;
            }
        }
        if (m_physicalDevice != VK_NULL_HANDLE) break;
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        std::cerr << "[Vulkan] No suitable GPU found\n";
        return false;
    }

    // Logical device
    float qPriority = 1.0f;
    VkDeviceQueueCreateInfo queueCI{};
    queueCI.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCI.queueFamilyIndex = m_queueFamily;
    queueCI.queueCount       = 1;
    queueCI.pQueuePriorities = &qPriority;

    const char* devExts[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo deviceCI{};
    deviceCI.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCI.queueCreateInfoCount    = 1;
    deviceCI.pQueueCreateInfos       = &queueCI;
    deviceCI.enabledExtensionCount   = 1;
    deviceCI.ppEnabledExtensionNames = devExts;

    if (vkCreateDevice(m_physicalDevice, &deviceCI, nullptr, &m_device) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create logical device\n";
        return false;
    }
    vkGetDeviceQueue(m_device, m_queueFamily, 0, &m_graphicsQueue);

    // Swap chain
    VkSurfaceCapabilitiesKHR caps{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &caps);

    uint32_t fmtCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &fmtCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFmts(fmtCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &fmtCount, surfaceFmts.data());

    // Prefer sRGB, fall back to whatever is first
    VkSurfaceFormatKHR chosenFmt = surfaceFmts[0];
    for (auto& f : surfaceFmts) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosenFmt = f;
            break;
        }
    }
    m_swapchainFormat = chosenFmt.format;
    m_swapchainExtent = (caps.currentExtent.width != UINT32_MAX)
                        ? caps.currentExtent
                        : VkExtent2D{ m_width, m_height };

    uint32_t imgCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imgCount > caps.maxImageCount)
        imgCount = caps.maxImageCount;

    VkSwapchainCreateInfoKHR scCI{};
    scCI.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    scCI.surface          = m_surface;
    scCI.minImageCount    = imgCount;
    scCI.imageFormat      = m_swapchainFormat;
    scCI.imageColorSpace  = chosenFmt.colorSpace;
    scCI.imageExtent      = m_swapchainExtent;
    scCI.imageArrayLayers = 1;
    scCI.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    scCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    scCI.preTransform     = caps.currentTransform;
    scCI.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    scCI.presentMode      = VK_PRESENT_MODE_FIFO_KHR;  // always available
    scCI.clipped          = VK_TRUE;

    if (vkCreateSwapchainKHR(m_device, &scCI, nullptr, &m_swapchain) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create swap chain\n";
        return false;
    }

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imgCount, nullptr);
    m_swapchainImages.resize(imgCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imgCount, m_swapchainImages.data());

    // Image views
    m_swapchainImageViews.resize(imgCount);
    for (uint32_t i = 0; i < imgCount; i++) {
        VkImageViewCreateInfo ivCI{};
        ivCI.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivCI.image                           = m_swapchainImages[i];
        ivCI.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        ivCI.format                          = m_swapchainFormat;
        ivCI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        ivCI.subresourceRange.levelCount     = 1;
        ivCI.subresourceRange.layerCount     = 1;
        if (vkCreateImageView(m_device, &ivCI, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS) {
            std::cerr << "[Vulkan] Failed to create image view\n";
            return false;
        }
    }

    // Render pass
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = m_swapchainFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo rpCI{};
    rpCI.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpCI.attachmentCount = 1;
    rpCI.pAttachments    = &colorAttachment;
    rpCI.subpassCount    = 1;
    rpCI.pSubpasses      = &subpass;
    rpCI.dependencyCount = 1;
    rpCI.pDependencies   = &dependency;

    if (vkCreateRenderPass(m_device, &rpCI, nullptr, &m_renderPass) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create render pass\n";
        return false;
    }

    // Shaders — compile GLSL to SPIR-V at runtime (mirrors DX12's D3DCompile)
    auto vertSPIRV = CompileGLSL(kVertGLSL, shaderc_glsl_vertex_shader,   "vert.glsl");
    auto fragSPIRV = CompileGLSL(kFragGLSL, shaderc_glsl_fragment_shader, "frag.glsl");
    if (vertSPIRV.empty() || fragSPIRV.empty()) return false;

    VkShaderModule vertMod = CreateShaderModule(vertSPIRV);
    VkShaderModule fragMod = CreateShaderModule(fragSPIRV);

    VkPipelineShaderStageCreateInfo stages[2]{};
    stages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vertMod;
    stages[0].pName  = "main";
    stages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fragMod;
    stages[1].pName  = "main";

    // Vertex input — matches the Vertex struct layout (position[3], color[4])
    VkVertexInputBindingDescription binding{};
    binding.binding   = 0;
    binding.stride    = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attrs[2]{};
    attrs[0].location = 0;
    attrs[0].binding  = 0;
    attrs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;     // position
    attrs[0].offset   = offsetof(Vertex, position);
    attrs[1].location = 1;
    attrs[1].binding  = 0;
    attrs[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;  // color
    attrs[1].offset   = offsetof(Vertex, color);

    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount   = 1;
    vertexInput.pVertexBindingDescriptions      = &binding;
    vertexInput.vertexAttributeDescriptionCount = 2;
    vertexInput.pVertexAttributeDescriptions    = attrs;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Viewport and scissor are set dynamically each frame
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode    = VK_CULL_MODE_NONE;
    rasterizer.frontFace   = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.lineWidth   = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                   | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments    = &blendAttachment;

    VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates    = dynStates;

    VkPipelineLayoutCreateInfo layoutCI{};
    layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    if (vkCreatePipelineLayout(m_device, &layoutCI, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create pipeline layout\n";
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCI.stageCount          = 2;
    pipelineCI.pStages             = stages;
    pipelineCI.pVertexInputState   = &vertexInput;
    pipelineCI.pInputAssemblyState = &inputAssembly;
    pipelineCI.pViewportState      = &viewportState;
    pipelineCI.pRasterizationState = &rasterizer;
    pipelineCI.pMultisampleState   = &multisample;
    pipelineCI.pColorBlendState    = &colorBlend;
    pipelineCI.pDynamicState       = &dynamicState;
    pipelineCI.layout              = m_pipelineLayout;
    pipelineCI.renderPass          = m_renderPass;
    pipelineCI.subpass             = 0;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &m_pipeline) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create graphics pipeline\n";
        return false;
    }

    // Shader modules are baked into the pipeline — no longer needed
    vkDestroyShaderModule(m_device, vertMod, nullptr);
    vkDestroyShaderModule(m_device, fragMod, nullptr);

    // Framebuffers — one per swap chain image
    m_framebuffers.resize(m_swapchainImageViews.size());
    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        VkFramebufferCreateInfo fbCI{};
        fbCI.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbCI.renderPass      = m_renderPass;
        fbCI.attachmentCount = 1;
        fbCI.pAttachments    = &m_swapchainImageViews[i];
        fbCI.width           = m_swapchainExtent.width;
        fbCI.height          = m_swapchainExtent.height;
        fbCI.layers          = 1;
        if (vkCreateFramebuffer(m_device, &fbCI, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            std::cerr << "[Vulkan] Failed to create framebuffer\n";
            return false;
        }
    }

    // Command pool + command buffer
    VkCommandPoolCreateInfo poolCI{};
    poolCI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolCI.queueFamilyIndex = m_queueFamily;
    if (vkCreateCommandPool(m_device, &poolCI, nullptr, &m_commandPool) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create command pool\n";
        return false;
    }

    VkCommandBufferAllocateInfo cmdAlloc{};
    cmdAlloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAlloc.commandPool        = m_commandPool;
    cmdAlloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAlloc.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(m_device, &cmdAlloc, &m_commandBuffer) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to allocate command buffer\n";
        return false;
    }

    // Vertex buffer — host-visible + coherent (no explicit flush needed)
    const VkDeviceSize bufSize = sizeof(Vertex) * 1024;

    VkBufferCreateInfo bufCI{};
    bufCI.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCI.size        = bufSize;
    bufCI.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(m_device, &bufCI, nullptr, &m_vertexBuffer) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create vertex buffer\n";
        return false;
    }

    VkMemoryRequirements memReqs{};
    vkGetBufferMemoryRequirements(m_device, m_vertexBuffer, &memReqs);

    VkMemoryAllocateInfo memAlloc{};
    memAlloc.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.allocationSize  = memReqs.size;
    memAlloc.memoryTypeIndex = FindMemoryType(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (vkAllocateMemory(m_device, &memAlloc, nullptr, &m_vertexBufferMemory) != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to allocate vertex buffer memory\n";
        return false;
    }
    vkBindBufferMemory(m_device, m_vertexBuffer, m_vertexBufferMemory, 0);

    // Sync objects
    VkSemaphoreCreateInfo semCI{};
    semCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // start signaled so first frame doesn't hang

    if (vkCreateSemaphore(m_device, &semCI,   nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &semCI,   nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence    (m_device, &fenceCI, nullptr, &m_inFlightFence)           != VK_SUCCESS) {
        std::cerr << "[Vulkan] Failed to create sync objects\n";
        return false;
    }

    std::cout << "[Vulkan] Initialization complete\n";
    return true;
}

// ─── BeginFrame ───────────────────────────────────────────────────────────────

void VulkanRenderer::BeginFrame() {
    // Wait for the previous frame to finish, then reset the fence
    vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &m_inFlightFence);

    vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,
                          m_imageAvailableSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);

    vkResetCommandBuffer(m_commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(m_commandBuffer, &beginInfo);

    VkClearValue clearColor{};
    clearColor.color = { { 0.1f, 0.1f, 0.1f, 1.0f } };

    VkRenderPassBeginInfo rpBI{};
    rpBI.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBI.renderPass        = m_renderPass;
    rpBI.framebuffer       = m_framebuffers[m_currentImageIndex];
    rpBI.renderArea.extent = m_swapchainExtent;
    rpBI.clearValueCount   = 1;
    rpBI.pClearValues      = &clearColor;
    vkCmdBeginRenderPass(m_commandBuffer, &rpBI, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkViewport viewport{};
    viewport.width    = static_cast<float>(m_swapchainExtent.width);
    viewport.height   = static_cast<float>(m_swapchainExtent.height);
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = m_swapchainExtent;
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

// ─── DrawMesh ─────────────────────────────────────────────────────────────────

void VulkanRenderer::DrawMesh(const Mesh* mesh) {
    // Upload vertex data directly — buffer is host-visible + coherent, no flush needed
    void* data = nullptr;
    vkMapMemory(m_device, m_vertexBufferMemory, 0,
                sizeof(Vertex) * mesh->GetVertexCount(), 0, &data);
    memcpy(data, mesh->GetVertices(), sizeof(Vertex) * mesh->GetVertexCount());
    vkUnmapMemory(m_device, m_vertexBufferMemory);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &m_vertexBuffer, &offset);
    vkCmdDraw(m_commandBuffer, mesh->GetVertexCount(), 1, 0, 0);
}

// ─── EndFrame ─────────────────────────────────────────────────────────────────

void VulkanRenderer::EndFrame() {
    vkCmdEndRenderPass(m_commandBuffer);
    vkEndCommandBuffer(m_commandBuffer);

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &m_imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask    = &waitStage;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &m_commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &m_renderFinishedSemaphore;
    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &m_renderFinishedSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &m_swapchain;
    presentInfo.pImageIndices      = &m_currentImageIndex;
    vkQueuePresentKHR(m_graphicsQueue, &presentInfo);
}

// ─── Shutdown ─────────────────────────────────────────────────────────────────

void VulkanRenderer::Shutdown() {
    std::cout << "[Vulkan] Shutting down...\n";
    vkDeviceWaitIdle(m_device);

    vkDestroyFence    (m_device, m_inFlightFence,           nullptr);
    vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);

    vkDestroyBuffer(m_device, m_vertexBuffer,       nullptr);
    vkFreeMemory   (m_device, m_vertexBufferMemory, nullptr);

    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    for (auto fb : m_framebuffers)
        vkDestroyFramebuffer(m_device, fb, nullptr);

    vkDestroyPipeline      (m_device, m_pipeline,       nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyRenderPass    (m_device, m_renderPass,     nullptr);

    for (auto iv : m_swapchainImageViews)
        vkDestroyImageView(m_device, iv, nullptr);

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroyDevice      (m_device,              nullptr);
    vkDestroySurfaceKHR  (m_instance, m_surface, nullptr);
    vkDestroyInstance    (m_instance,            nullptr);

    std::cout << "[Vulkan] Shutdown complete\n";
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props) const {
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }
    return UINT32_MAX;
}

VkShaderModule VulkanRenderer::CreateShaderModule(const std::vector<uint32_t>& spirv) const {
    VkShaderModuleCreateInfo ci{};
    ci.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = spirv.size() * sizeof(uint32_t);
    ci.pCode    = spirv.data();

    VkShaderModule mod = VK_NULL_HANDLE;
    vkCreateShaderModule(m_device, &ci, nullptr, &mod);
    return mod;
}
