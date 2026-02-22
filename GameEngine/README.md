# Game Engine — C++ from Scratch

A 3D game engine built in C++, targeting PC first. One system at a time.

---

## Why DX12 + Vulkan

Modern engines pick a graphics backend based on platform reach:

| Backend | Platform |
|---|---|
| DirectX 12 | Windows (and Xbox if licensed) |
| Vulkan | Windows, Linux, Steam Deck, Android, Mac via MoltenVK |

DX12 + Vulkan covers all open platforms without a third backend. The only meaningful gap is native iOS/Mac — Apple deprecated OpenGL, never adopted Vulkan, and requires Metal for first-class support.

**DX11 vs DX12:** DX11 is high-level — the driver manages memory and synchronization for you. DX12 is low-level — you do it yourself. More work, more control, better performance when done right.

**OpenGL vs Vulkan:** Same generational shift. OpenGL (1992) was designed for a single-threaded world. Vulkan (2016) is explicit — you manage everything. 5-10x more code to draw a triangle, but predictable performance and real multi-threading support.

---

## Architecture

A game engine is layered. Higher layers never talk to lower layers directly — only through abstractions:

```
Game Code          (entities, gameplay logic, scenes)
    ↓
Engine Systems     (physics, audio, input, asset manager)
    ↓
Renderer           (IRenderer — abstract interface)
    ↓
Graphics Backend   (DX12Renderer / VulkanRenderer)
    ↓
GPU Driver / Hardware
```

The renderer is a relatively thin layer. The surrounding systems — physics, audio, animation, asset pipeline, UI, scene management — are the majority of the codebase. In a full engine like Unreal, rendering is roughly 10-15% of the code.

---

## The IRenderer Interface

The engine uses a pure interface to decouple all engine systems from any specific graphics backend:

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual bool Init()       = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame()   = 0;
    virtual void Shutdown()   = 0;
};
```

A factory function selects the backend at compile time:

```cpp
// RendererFactory.cpp — the only file that names a concrete backend
#ifdef RENDERER_DX12
    IRenderer* CreateRenderer() { return new DX12Renderer(); }
#elif defined(RENDERER_VULKAN)
    IRenderer* CreateRenderer() { return new VulkanRenderer(); }
#endif
```

All engine and game code holds an `IRenderer*` and never names DX12 or Vulkan directly.

---

## How Professional Engines Do It

From Software ships Elden Ring on PC (DX12), PS5 (GNM), and Xbox (GDK). Their engine — believed to be called **Dantelion** — almost certainly has the same pattern: one abstract graphics device interface, one concrete implementation per platform, selected at compile time. Game code never names a backend directly.

Unreal Engine calls their abstraction layer the **RHI** (Rendering Hardware Interface), with `D3D12RHI` and `VulkanRHI` as backends.

---

## Console Support

Console backends (Xbox GDK, PlayStation GNM, Nintendo NVN) are gated by developer agreements, not technical complexity. The engine architecture already supports them — adding a console renderer is the same pattern as adding any other backend. The business agreement unlocks the platform; the code structure is already ready.

---

## Existing C++ Engines Worth Studying

| Engine | Backends | Notes |
|---|---|---|
| [**O3DE**](https://o3de.org) | DX12, Vulkan | Amazon's open source engine, enterprise-scale |
| [**Godot 4**](https://godotengine.org) | Vulkan | Fully open source, well-architected codebase |
| [**Wicked Engine**](https://github.com/turanszkij/WickedEngine) | DX11, DX12, Vulkan | Smaller, readable, has surrounding systems |
| [**The Forge**](https://github.com/ConfettiFX/The-Forge) | DX12, Vulkan, Metal | Production-quality rendering framework |
| [**bgfx**](https://github.com/bkaradzic/bgfx) | DX12, Vulkan, Metal, GL | Widely used rendering abstraction |
| [**Spartan Engine**](https://github.com/PanosK92/SpartanEngine) | DX12, Vulkan | Built explicitly for learning/research |
| [**Hazel**](https://github.com/TheCherno/Hazel) | Vulkan, DX12 | The Cherno's educational engine series |

---

## Project Structure

```
GameEngine/
  step01/              ← open a window with OpenGL (GLFW + GLAD)
  step02/              ← engine architecture overview
  step03/              ← engine boilerplate: IRenderer interface, DX12 + Vulkan support
  step04/              ← engine code becomes a consumable class by 1-N games
  ROADMAP.md
```
