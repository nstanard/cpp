cmake_minimum_required(VERSION 3.20)
project(Cupcake VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Renderer selection
# ---------------------------------------------------------------------------
set(RENDERER "VULKAN" CACHE STRING "Renderer backend: DX12 or VULKAN")
set_property(CACHE RENDERER PROPERTY STRINGS DX12 VULKAN)
message(STATUS "Cupcake — renderer backend: ${RENDERER}")

# ---------------------------------------------------------------------------
# External dependencies
# ---------------------------------------------------------------------------
find_package(glfw3 REQUIRED)

if(RENDERER STREQUAL "VULKAN")
    find_package(Vulkan REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(shaderc REQUIRED IMPORTED_TARGET shaderc)
endif()

# ---------------------------------------------------------------------------
# Sources — only the selected backend compiles
# ---------------------------------------------------------------------------
set(ENGINE_SOURCES
    src/Engine.cpp
    src/renderer/RendererFactory.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(RENDERER STREQUAL "DX12")
    list(APPEND ENGINE_SOURCES src/renderer/dx12/DX12Renderer.cpp)
elseif(RENDERER STREQUAL "VULKAN")
    list(APPEND ENGINE_SOURCES src/renderer/vulkan/VulkanRenderer.cpp)
endif()

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------
add_library(Cupcake STATIC ${ENGINE_SOURCES})

# BUILD_INTERFACE: path used when building inside this project
# INSTALL_INTERFACE: path used by consumers after cmake --install
target_include_directories(Cupcake
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

target_compile_definitions(Cupcake
    PRIVATE RENDERER_${RENDERER}
)

# GLFW is PUBLIC — Engine.h exposes GLFWwindow*, so consumers need GLFW too
target_link_libraries(Cupcake PUBLIC glfw)

if(RENDERER STREQUAL "VULKAN")
    target_link_libraries(Cupcake PRIVATE Vulkan::Vulkan PkgConfig::shaderc)
endif()

if(RENDERER STREQUAL "DX12")
    target_link_libraries(Cupcake PRIVATE d3d12 dxgi dxguid)
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS Cupcake
    EXPORT  CupcakeTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT CupcakeTargets
    FILE        CupcakeTargets.cmake
    NAMESPACE   Cupcake::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cupcake
)

configure_package_config_file(
    cmake/CupcakeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CupcakeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cupcake
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CupcakeConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Cupcake
)
