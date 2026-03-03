# Step 05 — Replace GLFW with Win32

Step04 used GLFW as a convenience library to create a window and get a Vulkan/DX12 surface. Step05 removes that dependency entirely and handles window creation directly through the Win32 API.

## What changed

### Engine public API (`engine/include/Engine.h`)

`Init` no longer takes a `GLFWwindow*`. It takes a `void*` — a platform-native window handle:

```cpp
// step04
bool Init(GLFWwindow* window);

// step05
bool Init(void* nativeWindowHandle);
```

`void*` keeps Engine.h free of any platform-specific type. The engine doesn't care what the handle points to — it passes it straight to the renderer. On Win32 that handle is an `HWND`.

### Vulkan surface creation (`engine/src/renderer/vulkan/VulkanRenderer.cpp`)

GLFW was doing two things for Vulkan that now happen manually:

**Instance extensions** — GLFW's `glfwGetRequiredInstanceExtensions` returned the right extensions for the current platform. Without it, we list them directly:

```cpp
const char* instanceExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,        // "VK_KHR_surface"
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,  // "VK_KHR_win32_surface"
};
```

**Surface creation** — GLFW's `glfwCreateWindowSurface` called `vkCreateWin32SurfaceKHR` internally. Now we call it directly:

```cpp
VkWin32SurfaceCreateInfoKHR surfaceCI{};
surfaceCI.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
surfaceCI.hinstance = GetModuleHandleW(nullptr);
surfaceCI.hwnd      = static_cast<HWND>(windowHandle);

vkCreateWin32SurfaceKHR(m_instance, &surfaceCI, nullptr, &m_surface);
```

`vulkan_win32.h` provides `VkWin32SurfaceCreateInfoKHR` and `vkCreateWin32SurfaceKHR`. It must be included after `windows.h` because it needs `HWND` and `HINSTANCE` to be defined.

### DX12 surface creation (`engine/src/renderer/dx12/DX12Renderer.cpp`)

DX12 was already Win32-native. The only change is that step04 had to extract the `HWND` from a `GLFWwindow*` using a GLFW extension function:

```cpp
// step04 — needed GLFW to get the Win32 handle
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
HWND hwnd = glfwGetWin32Window(static_cast<GLFWwindow*>(windowHandle));

// step05 — the handle already is an HWND
HWND hwnd = static_cast<HWND>(windowHandle);
```

### CMake (`engine/CMakeLists.txt`, `engine/cmake/CupcakeConfig.cmake.in`)

`glfw3` is no longer a dependency at any level:

```cmake
# removed
find_package(glfw3 REQUIRED)
target_link_libraries(Cupcake PUBLIC glfw)
```

Win32 is part of the Windows SDK — no `find_package` or `target_link_libraries` entry needed.

### Game layer (`snake/src/main.cpp`)

The game is now responsible for creating its own window using the Win32 API. GLFW's four lines become slightly more verbose Win32 calls, but the concepts are identical:

| GLFW | Win32 |
|---|---|
| `glfwInit()` | `RegisterClassExW(&wc)` |
| `glfwCreateWindow(...)` | `CreateWindowExW(...)` |
| `glfwWindowShouldClose(window)` | `msg.message != WM_QUIT` |
| `glfwPollEvents()` | `PeekMessageW` + `DispatchMessageW` |
| `glfwDestroyWindow` + `glfwTerminate` | `WM_DESTROY` → `PostQuitMessage(0)` |

The message loop uses `PeekMessage` (non-blocking) rather than `GetMessage` (blocking) because the game loop needs to run every frame regardless of whether Windows has a message to deliver:

```cpp
MSG msg = {};
while (msg.message != WM_QUIT) {
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if (msg.message == WM_QUIT) break;
    }
    engine.Update();  // runs every frame, not just when a message arrives
}
```

## Building

```bash
./build.sh vulkan -gcr   # generate, compile, run (Vulkan)
./build.sh dx12 -gcr     # generate, compile, run (DX12)
./build.sh vulkan -c     # recompile only
```
