#!/bin/bash
# build.sh — step05 two-project build runner
#
# Usage: ./build.sh <renderer> [-g] [-c] [-r]
#   renderer  vulkan | dx12
#   -g  Generate  — configure engine, build+install it, then configure snake
#   -c  Recompile — rebuild engine, reinstall, rebuild snake
#   -r  Run       — launch Snake.exe
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
INSTALL_DIR="$(cygpath -m "$PWD")/install-cupcake-$RENDERER_ARG"

while getopts "gcr" opt; do
    case $opt in
        g)
            echo "==> Generating engine ($RENDERER)..."
            cmake -B "$BUILD_DIR/engine" -G Ninja \
                  -DRENDERER="$RENDERER" \
                  -DCMAKE_PREFIX_PATH="$PREFIX" \
                  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
                  -S engine || exit 1

            echo "==> Building and installing engine..."
            cmake --build "$BUILD_DIR/engine" || exit 1
            cmake --install "$BUILD_DIR/engine" || exit 1

            echo "==> Generating snake ($RENDERER)..."
            cmake -B "$BUILD_DIR/snake" -G Ninja \
                  -DCMAKE_PREFIX_PATH="$PREFIX;$INSTALL_DIR" \
                  -S snake
            ;;
        c)
            echo "==> Compiling engine ($RENDERER)..."
            cmake --build "$BUILD_DIR/engine"

            echo "==> Installing engine to $INSTALL_DIR..."
            cmake --install "$BUILD_DIR/engine"

            echo "==> Compiling snake ($RENDERER)..."
            cmake --build "$BUILD_DIR/snake"
            ;;
        r)
            echo "==> Running ($RENDERER)..."
            "./$BUILD_DIR/snake/Snake.exe"
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
