# C++ & Game Development Roadmap

**Goal**: Learn C++ through the lens of video game development, leading toward Unreal Engine.

**Background**: 10+ years front-end/full-stack dev. Dabbled in C and Java in college. Strong programming fundamentals, new to compiled/systems languages.

---

## Phase 1: C++ Fundamentals

### Stage 1 — The Mental Model Shift
> The concepts that feel most alien coming from JS/TypeScript/web dev.

- [ ] Understand the compilation pipeline (source → object → executable)
- [ ] Understand that there is no runtime, no garbage collector, no JIT
- [ ] Stack vs heap memory — what lives where and why it matters
- [ ] Why types are strict and what that buys you
- [ ] Intro to pointers — the concept that trips up every web dev

**Resources**
- [learncpp.com — Chapter 0 & 1](https://www.learncpp.com/)

---

### Stage 2 — Core Language Basics
> The fundamentals. Most of this will feel familiar with some new syntax.

- [ ] Variables and primitive types (`int`, `float`, `double`, `char`, `bool`)
- [ ] `const` and why it matters more than in JS
- [ ] Functions — declaration vs definition, header files (`.h` / `.cpp` split)
- [ ] Control flow (`if`, `for`, `while`, `switch`)
- [ ] `std::string` and character arrays
- [ ] Basic I/O with `std::cin` / `std::cout`

**Resources**
- [learncpp.com — Chapters 1–9](https://www.learncpp.com/)

**Mini Project**: Write a CLI program that takes user input and responds (text adventure room, simple quiz, etc.)

---

### Stage 3 — Pointers & Memory Management
> The hardest mental shift. Take your time here.

- [ ] What a pointer is and how to read pointer syntax (`int* p`, `*p`, `&x`)
- [ ] References (`&`) vs pointers — when to use which
- [ ] Dynamic allocation with `new` and `delete`
- [ ] Memory leaks — what they are and how to cause/fix them
- [ ] Modern smart pointers: `std::unique_ptr`, `std::shared_ptr`
- [ ] RAII — the core C++ pattern for managing resource lifetimes

**Resources**
- [learncpp.com — Chapters 9, 11–12](https://www.learncpp.com/)

**Mini Project**: Build a simple dynamic array or linked list by hand

---

### Stage 4 — OOP in C++
> You know OOP — learn how C++ does it differently.

- [ ] Classes: fields, methods, access specifiers (`public`, `private`, `protected`)
- [ ] Constructors and destructors (no garbage collector = destructor matters)
- [ ] `this` pointer
- [ ] Inheritance and `virtual` functions (runtime polymorphism)
- [ ] Pure virtual functions and abstract classes (interfaces in C++)
- [ ] Operator overloading basics

**Resources**
- [learncpp.com — Chapters 13–17](https://www.learncpp.com/)

**Mini Project**: Model a game entity system — `Entity` base class with `Player` and `Enemy` subclasses

---

### Stage 5 — Standard Library & Putting It Together
> The tools you'll reach for constantly.

- [ ] `std::vector` — the workhorse array
- [ ] `std::map` and `std::unordered_map`
- [ ] Iterators and range-based `for` loops
- [ ] `std::optional`, `std::variant` (modern C++ patterns)
- [ ] Lambdas and `std::function`
- [ ] Basic file I/O

**Mini Project**: Build a game launcher that goes into a full-screen menu system. The menu includes a **New Game** option that prompts the user to enter a name via standard I/O, plus placeholders for Load Game and Quit. There will be no real game or graphics usage other than the menu system for this project. We will use this as a foundation for all other c++ or graphics mini projects.

---

## Phase 2: Game Math & Pre-Engine Concepts
> *Not started yet — will be detailed once Phase 1 is complete.*

- Vectors and matrices (2D/3D math)
- The game loop pattern
- Delta time and frame independence
- Basic collision concepts

---

## Phase 3: Unreal Engine C++
> *Not started yet — will be detailed once Phase 2 is complete.*

- Unreal's build system and project structure
- Blueprint vs C++ — when to use which
- Actors, Components, the GameMode/GameState/PlayerController hierarchy
- Unreal's coding conventions and macro system (`UCLASS`, `UPROPERTY`, etc.)
- Interfacing C++ with Blueprints

---

## Tools

| Tool | Purpose | Status |
|------|---------|--------|
| Visual Studio 2022 Community | IDE + MSVC compiler | [ ] Install |
| VS Code + C/C++ extension | Lightweight editor option | [ ] Optional |
| learncpp.com | Primary learning resource | [ ] In progress |
| Unreal Engine 5 | Game engine (Phase 3) | [x] Installed |

---

## Progress Log

| Date | Notes |
|------|-------|
| — | Roadmap created |
