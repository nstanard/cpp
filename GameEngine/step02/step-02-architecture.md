# Step 2: Understand the Engine Architecture Before Writing It

Before writing any 3D code, understand how a game engine is structured and why.
This is the mental map you'll be building toward.

---

## How a Game Engine Is Layered

```
Game Code          (entities, gameplay logic, scenes)
    ↓
Engine Systems     (physics, audio, input, asset manager)
    ↓
Renderer           (YOUR abstraction layer — IRenderer interface)
    ↓
Graphics Backend   (the actual API that talks to the GPU)
    ↓
GPU Driver         (written by Nvidia/AMD — you never touch this)
    ↓
GPU Hardware
```

You write everything above the graphics backend. You do not reimplement the backend —
you call into it. The backend is provided by the platform.

---

## Example: How From Software Likely Does It

From Software ships Elden Ring on PC (DX12), PS5 (GNM), and Xbox (GDK). They almost
certainly have an abstract graphics device interface, with platform backends selected
at compile time — the same pattern you're building here.

```cpp
// Somewhere in their engine (not public — this is an educated reconstruction):
class IGfxDevice {
public:
    virtual void BeginFrame() = 0;
    virtual void Submit(const DrawCall&) = 0;
    virtual void EndFrame() = 0;
};

// Platform-specific implementations, one per target:
class DX12GfxDevice  : public IGfxDevice { ... };  // Windows / Xbox
class GNMGfxDevice   : public IGfxDevice { ... };  // PlayStation
class NVNGfxDevice   : public IGfxDevice { ... };  // Nintendo Switch
```

Their game code — the stuff that draws a boss, renders a fog wall, submits a particle
effect — never calls DX12 or GNM directly. It calls `IGfxDevice`. The backend is
swapped out entirely per platform build.

The engine name used internally at From Software is believed to be **Dantelion**
(referenced in job postings and asset metadata). It is a proprietary engine, not
licensed middleware.

---

## The Graphics Backends

| Backend | Platform | License required? |
|---|---|---|
| **OpenGL** | Windows, Linux, (deprecated on Mac) | No |
| **DirectX 11** | Windows PC | No — ships free with Windows SDK |
| **DirectX 12** | Windows PC | No — ships free with Windows SDK |
| **Vulkan** | Windows, Linux, Android, Mac via MoltenVK | No — open standard |
| **Metal** | Mac, iPhone, iPad | No — but Apple hardware only |
| **OpenGL ES** | Android, iOS (legacy) | No — open standard |
| **WebGL / WebGL 2** | Web browsers (legacy) | No — open standard |
| **WebGPU** | Web browsers | No — open standard |
| **NVN** | Nintendo Switch | Yes — Nintendo developer agreement + dev kit |
| **Xbox GDK** | Xbox console | Yes — Microsoft ID@Xbox program + dev kit |
| **GNM/GNMX** | PlayStation console | Yes — Sony developer agreement + dev kit |

The license wall is the **console platform**, not the API. DirectX 12 on PC is
completely free. It's only when targeting Xbox hardware that a console license is needed.

---

## This Engine Targets: DirectX 12 + Vulkan

| Backend | Covers |
|---|---|
| **DirectX 12** | Windows |
| **Vulkan** | Windows, Linux, Steam Deck, Android, Mac (via MoltenVK) |

This gives broad PC reach and a clear console path without a third backend.

---

## How Multi-Backend Works in Code

CMake selects the backend at configure time:
```bash
cmake -B build-dx12    -DRENDERER=DX12
cmake -B build-vulkan  -DRENDERER=VULKAN
```

C++ compiles only the relevant backend via `#ifdef`:
```cpp
#ifdef RENDERER_DX12
    #include "renderer/dx12/DX12Renderer.h"
#elif RENDERER_VULKAN
    #include "renderer/vulkan/VulkanRenderer.h"
#endif
```

All engine systems talk to a single abstract interface — never a backend directly:
```cpp
// engine code calls this — doesn't know or care what's underneath
IRenderer* renderer = CreateRenderer();
renderer->DrawMesh(...);
```

---

## Target Folder Structure

```
GameEngine/
  src/
    renderer/
      IRenderer.h              ← abstract interface (platform-agnostic)
      dx12/
        DX12Renderer.h
        DX12Renderer.cpp       ← compiled only for Windows/Xbox builds
      vulkan/
        VulkanRenderer.h
        VulkanRenderer.cpp     ← compiled only for Vulkan builds
    engine/
      (physics, audio, input, etc. — all call IRenderer only)
  CMakeLists.txt
```

This is the same pattern Unreal Engine uses — their abstract layer is called the
`RHI` (Rendering Hardware Interface), with `D3D12RHI` and `VulkanRHI` as backends.

---

## What Step 3 Will Do

Start implementing this structure — create `IRenderer`, a stub `VulkanRenderer`,
and wire up CMake to select between backends at build time.
