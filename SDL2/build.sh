#!/bin/bash
# build.sh — project build runner
#
# WHY THIS EXISTS:
#   CMake requires 2-3 commands to build and run a project. This script wraps
#   them behind single-letter flags so you don't retype the full commands every
#   time. Each step0 of the roadmap lives in its own folder with its own
#   CMakeLists.txt — pass the folder name as the first argument.
#
# Usage: ./build.sh <folder> [-g] [-c] [-r]
#   -g  Generate build files — runs cmake to configure the project and write
#       build files into <folder>/build/. Only needed once per project, or any
#       time CMakeLists.txt changes.
#   -c  Compile — runs cmake --build to compile source files into an executable.
#       Run this every time you change code.
#   -r  Run — executes the compiled game.exe from the build folder.
#
# Examples:
#   ./build.sh step01 -g          # generate only
#   ./build.sh step01 -c          # compile only
#   ./build.sh step01 -gcr        # generate, compile, and run
#   ./build.sh step01 -g -c -r    # same as above

FOLDER=$1
shift

if [ -z "$FOLDER" ]; then
    echo "Usage: ./build.sh <folder> [-g] [-c] [-r]"
    exit 1
fi

if [ ! -d "$FOLDER" ]; then
    echo "Error: folder '$FOLDER' not found"
    exit 1
fi

while getopts "gcr" opt; do
    case $opt in
        g)
            echo "==> [$FOLDER] Generating build files..."
            cmake -B "$FOLDER/build" -G Ninja -S "$FOLDER"
            ;;
        c)
            echo "==> [$FOLDER] Compiling..."
            cmake --build "$FOLDER/build"
            ;;
        r)
            echo "==> [$FOLDER] Running..."
            "./$FOLDER/build/game.exe"
            ;;
        \?)
            echo "Unknown flag: -$OPTARG"
            exit 1
            ;;
    esac
done
