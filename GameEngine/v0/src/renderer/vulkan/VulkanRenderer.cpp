// Stub — no <vulkan/vulkan.h> yet. Real Vulkan calls come in a future step.

#include "renderer/vulkan/VulkanRenderer.h"
#include <iostream>

bool VulkanRenderer::Init()       { std::cout << "[Vulkan] Init\n";       return true; }
void VulkanRenderer::BeginFrame() { std::cout << "[Vulkan] BeginFrame\n";              }
void VulkanRenderer::EndFrame()   { std::cout << "[Vulkan] EndFrame\n";                }
void VulkanRenderer::Shutdown()   { std::cout << "[Vulkan] Shutdown\n";                }
