
https://www.learncpp.com/cpp-tutorial/introduction-to-programming-languages/

Programming language hierarchy
===============================
 - Machine Language: Machine language instructions (like 10110000 01100001) are ideal for a CPU. 
 
 - Assembly Language: Human-readable abstraction of machine language

 - Low-level language: Little or no abstraction from a computers Instruction Set Architecture (ISA). Memory or underlying physical hardware; commands or functions in the language are structurally similar to the processor's instructions.
 
 - High-level language: C, C++, to address the downsides of low-level languages




https://www.learncpp.com/cpp-tutorial/introduction-to-cplusplus/

Intro to C / C++
================= 

Dennis Ritchie at Bell Telephone Labs developed C in 1972. The primary goal being was to produce a minimalistic language that was easy to compile, allowed efficient access to memory, produced efficient code, and was self-contained (not reliant on other programs).

Assembly produces programs that can only run on specific CPUs, but C has excellent portability thanks to any standards-compliant C compiler producing code that runs on entirely different CPU architectures and operating systems.

C++ was developed by Bjarne Stroustrup at Bell Labs in 1979. It adds many new features to the C language and is largely based on C, though not a strict superset — some valid C code is not valid C++. TypeScript is a valid superset of JavaScript for instance.

C++ ws standardized in 1998 by the ISO committee. C++ 11 is currently (2026) considered the new baseline version of the language.



https://www.learncpp.com/cpp-tutorial/introduction-to-cpp-development/

Intro to C++ Development
========================


https://www.learncpp.com/cpp-tutorial/introduction-to-the-compiler-linker-and-libraries/


Hello World First Steps:

 - Install the C++ compiler. Compiler creates object files containing the machine language instructions and the linker validates and integrates them together into an .exe output file.
 - Create a main.cpp file, write valid main driver syntax with standard output "Hello World"

---

Hello World — Native Compile (Windows x86-64)
==============================================

Tools: MinGW-w64 (GCC for Windows) or MSVC (Visual Studio)

1. Install MSYS2 from https://www.msys2.org, which provides GCC/G++ for Windows.
   Run in the MSYS2 terminal:
       pacman -S mingw-w64-ucrt-x86_64-gcc

2. Create main.cpp:

       #include <iostream>

       int main() {
           std::cout << "Hello, World!" << std::endl;
           return 0;
       }

3. Compile natively (targets your current machine — x86-64 Windows):
       g++ main.cpp -o hello.exe

4. Run it:
       ./hello.exe


Hello World — Cross Compile to ARM64 Linux
===========================================

Cross-compilation means building on your Windows x86-64 machine, but producing
a binary that runs on a different architecture (e.g. a Raspberry Pi running Linux).

Tools: Clang/LLVM (best cross-compile support on Windows) + a sysroot or linker for the target

Option A: Using Clang with a target triple
------------------------------------------
Clang has multi-target support built in. You tell it the target via --target:

1. Install LLVM/Clang from https://llvm.org or via winget:
       winget install LLVM.LLVM

2. Compile targeting 64-bit ARM Linux:
       clang++ --target=aarch64-linux-gnu main.cpp -o hello_arm64

   The target triple format is:  <arch>-<vendor>-<os>-<abi>
   Common targets:
       x86_64-pc-windows-msvc     → Windows 64-bit
       x86_64-pc-linux-gnu        → Linux 64-bit
       aarch64-linux-gnu           → ARM64 Linux (Raspberry Pi 4+)
       arm-linux-gnueabihf         → ARM 32-bit Linux (older Pi)
       wasm32-unknown-unknown      → WebAssembly

   Note: Cross-compilation often also requires a matching sysroot (the target
   platform's standard library headers/binaries). For simple programs with no
   OS calls this can be skipped, but real projects need it.

Option B: WSL2 (Windows Subsystem for Linux)
--------------------------------------------
For Linux targets specifically, the easiest path on Windows is WSL2.
You get a real Linux environment and can use its native GCC toolchains:

1. Install WSL2:  wsl --install
2. Inside WSL, install a cross-compiler for ARM:
       sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
3. Cross-compile:
       aarch64-linux-gnu-g++ main.cpp -o hello_arm64
4. Transfer to target machine and run natively there.


Key Concepts:
 - One compiler run = one target. To support multiple platforms, build once per target.
 - The target triple tells the compiler the architecture, OS, and ABI to emit code for.
 - Clang is the most flexible for cross-compilation from a single install.
 - GCC cross-compilers are separate installs per target (e.g. gcc-aarch64-linux-gnu).
 - The output binary is NOT runnable on your machine if it targets a different architecture.
