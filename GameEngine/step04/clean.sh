#!/bin/bash

# Remove DX12 build directory
rm -rf build-dx12

# Remove Vulkan build directory
rm -rf build-vulkan

# Remove CMake and Ninja files in step03 root
rm -f CMakeCache.txt
rm -f build.ninja
rm -f cmake_install.cmake

# Remove CMakeFiles directory
rm -rf CMakeFiles

echo "Build output cleaned."
