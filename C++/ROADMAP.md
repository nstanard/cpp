# C++ — Language Roadmap

**Goal**: Understand C++ from the ground up, systematically.
**Philosophy**: Each topic produces something you can run and inspect — no step is purely theoretical.

---

## 1. Types, Variables, and Control Flow
The absolute basics — how C++ represents data and makes decisions.
- [ ] Primitive types: `int`, `float`, `double`, `bool`, `char`
- [ ] Variables, assignment, and initialization
- [ ] `if` / `else`, `switch`
- [ ] `for`, `while`, `do-while`
- [ ] Functions: parameters, return types, overloading

---

## 2. Memory — The Most Important Topic in C++
What makes C++ fundamentally different from most languages.
- [ ] The stack vs the heap
- [ ] Pointers: what they are, how to read `int*`, dereferencing with `*`, address-of with `&`
- [ ] References: `int&` vs `int*` — when to use each
- [ ] `new` and `delete` — manual heap allocation
- [ ] What happens when you forget to `delete` (memory leaks)

---

## 3. Arrays and Strings
- [ ] Raw arrays: `int arr[10]`, why they decay to pointers
- [ ] `std::string` — the safe way to handle text
- [ ] `std::array` and `std::vector` — prefer these over raw arrays

---

## 4. Classes and Objects
How C++ groups data and behavior together.
- [ ] `struct` vs `class` (the only real difference: default access)
- [ ] Member variables and methods
- [ ] Constructors and destructors
- [ ] `public`, `private`, `protected`
- [ ] `const` methods

---

## 5. Inheritance and Virtual Functions
How C++ enables polymorphism — the mechanism behind `IRenderer`.
- [ ] Base classes and derived classes
- [ ] `virtual` functions and why they exist
- [ ] Pure virtual functions (`= 0`) and abstract classes / interfaces
- [ ] Virtual destructors — why forgetting one is undefined behavior
- [ ] `override` keyword

---

## 6. Memory Management — The Right Way (Modern C++)
Manual `new`/`delete` is error-prone. Modern C++ has better tools.
- [ ] RAII: resource acquisition is initialization
- [ ] `std::unique_ptr` — single-owner heap allocation
- [ ] `std::shared_ptr` — shared ownership with reference counting
- [ ] When to use each, and when a raw pointer is still fine

---

## 7. The Standard Library (STL)
Containers and algorithms you'll use constantly.
- [ ] `std::vector` — resizable array
- [ ] `std::unordered_map` — hash map
- [ ] `std::string` in depth
- [ ] Iterators and range-based `for`
- [ ] Common algorithms: `std::sort`, `std::find`, `std::transform`

---

## 8. Templates
Write code that works for any type without duplicating it.
- [ ] Function templates
- [ ] Class templates
- [ ] `typename` vs `class`
- [ ] When templates are the right tool (and when they aren't)

---

## 9. Move Semantics and Performance
What makes modern C++ (C++11 and later) fast.
- [ ] Copying vs moving — the difference and why it matters
- [ ] `std::move`
- [ ] Rvalue references (`&&`)
- [ ] When the compiler moves automatically

---

## 10. Build Systems and Project Structure
How C++ code gets turned into executables.
- [ ] Compilation units and the linker
- [ ] Header files vs source files — why the split exists
- [ ] Include guards and `#pragma once`
- [ ] CMake fundamentals
- [ ] Separating a project into libraries

---
