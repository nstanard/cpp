# C++ & Game Development Roadmap

**Goal**: Learn C++ through the lens of SDL2 game development, leading toward Unreal Engine.

**Philosophy**: Every C++ concept is introduced in the context of building something real with SDL2.
You always have a window. You always have a game loop. There are no abstract exercises disconnected
from the goal.

**Background**: 10+ years front-end/full-stack dev. Dabbled in C and Java in college. Strong
programming fundamentals, new to compiled/systems languages.

---

## Phase 1: Setup & First Window
> Get SDL2 running and understand what just happened.

- [X] Understand the compilation pipeline — source → object → executable
- [X] Install MSYS2 and GCC for Windows
- [X] Install SDL2 via MSYS2: `pacman -S mingw-w64-ucrt-x86_64-SDL2`
- [X] Understand what a library is — all packaged libraries contain headers (`.h`), which are the contract (what exists), and compiled binaries (`.dll` / `.a`), which are the implementation (how it works). Pre-compiled so you don't rebuild the library every time. SDL2 is just a library for windowing, rendering, input, audio, and networking (via extensions) — the same concept, just a much larger one.
- [ ] Link SDL2 to your project — the `-l` flag means "link this library." Every time you compile
  you must pass `-lSDL2 -lSDL2main` or the linker won't find SDL2's code and you'll get "undefined
  reference" errors. Full command: `g++ main.cpp -o game.exe -lSDL2 -lSDL2main`
    To avoid retyping this every build, use one of these approaches (in order of complexity):
    - Bash script — just the raw `g++` command in a `.sh` file. Run with `./build.sh`. No smarts.
    - Makefile — a recipe file that defines named targets (e.g. `make build`, `make clean`).
      Only recompiles files that changed since the last build. No install needed — `make` ships
      with MSYS2. Still manual about what flags and files to include.
    - CMake — a full build system generator. You describe your project at a high level
      (`find_package`, `target_link_libraries`) and CMake figures out the flags, file tracking,
      and generates the underlying Makefile or Ninja build for you. Handles dependencies,
      multiple files, and cross-platform builds. Used by large C++ projects and Unreal Engine.

  **We are using CMake because it's harder to learn** — the same build system used by large C++ projects and Unreal Engine.

  Setup (once):
    1. Install CMake and Ninja via MSYS2:
       `pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja`
    2. Create a `CMakeLists.txt` in your project root (tells CMake what to build and what to link)

  Every build after that:
    `cmake -B build -G Ninja`   ← generate build files into a build/ folder (once per project)
    `cmake --build build`       ← compile
    `./build/game.exe`          ← run

  The `CMakeLists.txt` for an SDL2 project looks like:
    ```
    cmake_minimum_required(VERSION 3.20)
    project(game)
    set(CMAKE_CXX_STANDARD 17)
    find_package(SDL2 REQUIRED)
    add_executable(game main.cpp)
    target_include_directories(game PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(game SDL2::SDL2main SDL2::SDL2)
    ```
- [ ] Create a window with `SDL_CreateWindow` and `SDL_CreateRenderer`
- [ ] The SDL2 init / quit lifecycle (`SDL_Init`, `SDL_Quit`)
- [ ] Understand there is no runtime, no garbage collector — you own everything you create

**Mini Project**: Fullscreen window that opens, shows a solid color, and closes cleanly on ESC.
This is your Hello World. Every future project builds on this shell.

---

## Phase 2: Core C++ in the Game Loop
> The language fundamentals — learned by making the window do things.

- [ ] Variables and primitive types (`int`, `float`, `double`, `bool`, `Uint8`) — used as colors, sizes, flags
- [ ] `const` — window dimensions, color values, speed constants that should never change
- [ ] The game loop: poll events → update state → render — the heartbeat of every game
- [ ] Control flow (`if`, `for`, `while`, `switch`) — for input handling and game logic
- [ ] Functions — breaking the game loop into `handleInput()`, `update()`, `render()`
- [ ] `std::string` and basic I/O (`std::cin` / `std::cout`) — for debug output and later menus
- [ ] Keyboard and mouse input with `SDL_PollEvent`, `SDL_KEYDOWN`, `SDL_QUIT`

**Mini Project**: Fullscreen window where keyboard keys cycle through background colors,
ESC quits. R=Red, G=Green, B=Blue, Space=White. Clean function separation.

---

## Phase 3: Pointers & Memory — SDL2 Forces You to Learn This
> SDL2 uses raw pointers everywhere. You can't avoid this.

- [ ] What a pointer is — `SDL_Window*`, `SDL_Renderer*`, `SDL_Texture*` are all pointers
- [ ] Pointer syntax: `int* p`, `*p` (dereference), `&x` (address-of)
- [ ] References (`&`) vs pointers — when SDL2 wants one vs the other
- [ ] `nullptr` — what it means when SDL returns null (failure / missing resource)
- [ ] `new` and `delete` — manual heap allocation, and why SDL does this for you via its own allocator
- [ ] Memory leaks — `SDL_DestroyWindow`, `SDL_DestroyRenderer`, `SDL_DestroyTexture` must be called
- [ ] RAII — wrapping SDL resources in structs/classes so cleanup is automatic
- [ ] Smart pointers: `std::unique_ptr` with a custom deleter for SDL resources

**Mini Project**: Refactor the color-cycling window to use RAII wrappers for
`SDL_Window` and `SDL_Renderer`. No raw cleanup calls in `main`.

---

## Phase 4: OOP — Building a Game Object System
> You know OOP. Learn how C++ does it, and why it matters for games.

- [ ] Classes: fields, methods, access specifiers (`public`, `private`, `protected`)
- [ ] Constructors and destructors — critical when objects own SDL resources
- [ ] `this` pointer
- [ ] Inheritance and `virtual` functions — `Entity` base with `update()` and `render()` as virtual methods
- [ ] Pure virtual functions and abstract classes — `Entity` is never instantiated directly
- [ ] Operator overloading basics — `Vec2 + Vec2`, `Vec2 * float` for position math

**Mini Project**: Entity system — abstract `Entity` base class with `Player` and `Enemy`
subclasses. Each has position, renders a colored rectangle, and moves independently.

---

## Phase 5: STL — Entities, Assets, and Save Files
> The standard library tools you'll use in every project.

- [ ] `std::vector` — a list of entities, a list of bullets, a list of rooms
- [ ] `std::map` and `std::unordered_map` — keyed asset registry, input binding maps
- [ ] Iterators and range-based `for` loops — iterating entities and cleaning up dead ones
- [ ] `std::optional` — for nullable game state (current room, selected item, etc.)
- [ ] `std::variant` — for game state machine (MenuState, PlayState, PauseState)
- [ ] Lambdas and `std::function` — event callbacks, sort comparators
- [ ] File I/O with `std::fstream` — save and load game data

**Mini Project**: Full-screen game launcher with a proper SDL2 menu system (rendered text,
keyboard navigation). New Game prompts for a player name (I/O), saves it to a file.
Load Game reads it back. Quit exits. This becomes the shell for all future projects.

---

## Phase 6: 2D Rendering — Sprites, Text, and Animation
> Making it look like a real game.

- [ ] Load images with `SDL_image` — PNG/JPG to `SDL_Texture*`
- [ ] `SDL_Rect` — position, size, source/destination rectangles for rendering
- [ ] Sprite sheets and frame animation — advancing frames with delta time
- [ ] Render text with `SDL_ttf` — load a `.ttf` font, render strings to texture
- [ ] Camera / viewport — separating world coordinates from screen coordinates
- [ ] Draw order / z-ordering — background first, entities second, UI last

**Mini Project**: Player sprite that walks (animated) around the screen. Camera follows.
UI shows player name (loaded from save file) and position in screen corner.

---

## Phase 7: Game Math & Movement
> The math behind every game mechanic.

- [ ] 2D vectors — `Vec2` struct with position, velocity, direction
- [ ] Euler integration — `position += velocity * deltaTime`
- [ ] Delta time with `SDL_GetTicks64()` — frame-rate-independent movement
- [ ] Basic trigonometry — angles, `sin`/`cos` for directional movement and aim
- [ ] AABB collision detection — axis-aligned bounding box overlap test
- [ ] Collision response — stop on contact, slide along walls

**Mini Project**: Player moves with WASD, collides with static wall rectangles,
can't pass through them. Velocity-based movement, fully delta-time independent.

---

## Phase 8: Game Architecture & Patterns
> Structuring code that can grow without becoming spaghetti.

- [ ] Scene / state manager — clean transitions between menu, gameplay, pause, game-over
- [ ] Game object lifecycle — spawn, update, destroy with deferred cleanup
- [ ] Simple component pattern — separating `PhysicsComponent`, `RenderComponent`, `InputComponent`
- [ ] Asset manager — load once, reuse textures/fonts across scenes
- [ ] Event system — decouple systems with a simple message bus
- [ ] Basic UI system — buttons, labels, layout for menus and HUD

**Mini Project**: A complete small 2D game — top-down or platformer — built on all prior
projects. Menu → gameplay → win/lose → back to menu. Enemies, collectibles, a score,
and a save file. Shippable as a `.exe`.

---

## Tools

| Tool | Purpose | Status |
|------|---------|--------|
| MSYS2 + MinGW-w64 | GCC compiler for Windows | [ ] Install |
| SDL2 | Windowing, input, rendering | [ ] Install via MSYS2 |
| SDL2_image | PNG/JPG texture loading | [ ] Install via MSYS2 |
| SDL2_ttf | TrueType font rendering | [ ] Install via MSYS2 |
| VS Code + C/C++ extension | Editor with IntelliSense | [ ] Configure |
| Unreal Engine 5 | Game engine (Phase 3) | [x] Installed |
| learncpp.com | C++ reference alongside projects | [ ] In progress |

---

## Progress Log

| Date | Notes |
|------|-------|
| 2026-02-21 | Roadmap created, roadmap rebuilt around SDL2 |
