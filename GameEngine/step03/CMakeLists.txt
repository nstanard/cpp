cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Backend selection ─────────────────────────────────────────────────────────
set(RENDERER "VULKAN" CACHE STRING "Graphics backend: DX12 or VULKAN")
set_property(CACHE RENDERER PROPERTY STRINGS DX12 VULKAN)

message(STATUS "Building with RENDERER=${RENDERER}")

# ── GLFW (always required — window creation is backend-agnostic) ──────────────
find_package(glfw3 REQUIRED)

# ── Common sources ────────────────────────────────────────────────────────────
set(ENGINE_SOURCES
    src/main.cpp
    src/renderer/RendererFactory.cpp
)

# ── Backend-specific sources and configuration ────────────────────────────────
if(RENDERER STREQUAL "DX12")
    list(APPEND ENGINE_SOURCES
        src/renderer/dx12/DX12Renderer.cpp
    )
elseif(RENDERER STREQUAL "VULKAN")
    find_package(Vulkan REQUIRED)
    list(APPEND ENGINE_SOURCES
        src/renderer/vulkan/VulkanRenderer.cpp
    )
else()
    message(FATAL_ERROR "Unknown RENDERER '${RENDERER}'. Must be DX12 or VULKAN.")
endif()

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(engine ${ENGINE_SOURCES})

target_include_directories(engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(engine PRIVATE RENDERER_${RENDERER})

# ── Link libraries ────────────────────────────────────────────────────────────
target_link_libraries(engine PRIVATE glfw)

if(RENDERER STREQUAL "DX12")
    target_link_libraries(engine PRIVATE d3d12 dxgi d3dcompiler)
elseif(RENDERER STREQUAL "VULKAN")
    target_link_libraries(engine PRIVATE Vulkan::Vulkan)
endif()
