#!/bin/bash
# build.sh — v0 engine build runner
#
# Usage: ./build.sh <renderer> [-g] [-c] [-r]
#   renderer  vulkan | dx12
#   -g  Generate  — configure cmake and write build files
#   -c  Compile   — build the executable
#   -r  Run       — launch engine.exe
#
# Examples:
#   ./build.sh vulkan -gcr     # generate, compile, and run (Vulkan)
#   ./build.sh dx12 -gcr       # generate, compile, and run (DX12)
#   ./build.sh vulkan -c       # recompile only

export PATH="/c/msys64/ucrt64/bin:$PATH"
PREFIX="/c/msys64/ucrt64"

RENDERER_ARG=$1
shift

case "$RENDERER_ARG" in
    vulkan) RENDERER="VULKAN" ;;
    dx12)   RENDERER="DX12"   ;;
    *)
        echo "Error: renderer must be 'vulkan' or 'dx12'"
        echo "Usage: ./build.sh <renderer> [-g] [-c] [-r]"
        exit 1
        ;;
esac

BUILD_DIR="build-$RENDERER_ARG"

while getopts "gcr" opt; do
    case $opt in
        g)
            echo "==> Generating ($RENDERER)..."
            cmake -B "$BUILD_DIR" -G Ninja \
                  -DRENDERER="$RENDERER" \
                  -DCMAKE_PREFIX_PATH="$PREFIX" \
                  -S .
            ;;
        c)
            echo "==> Compiling ($RENDERER)..."
            cmake --build "$BUILD_DIR"
            ;;
        r)
            echo "==> Running ($RENDERER)..."
            "./$BUILD_DIR/engine.exe"
            ;;
        \?)
            echo "Unknown flag: -$OPTARG"
            exit 1
            ;;
    esac
done

if [ $OPTIND -eq 1 ]; then
    echo "Usage: ./build.sh <renderer> [-g] [-c] [-r]"
fi
