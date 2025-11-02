#!/bin/bash

# Build script for LLVM DSL Compiler

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=== Building LLVM DSL Compiler ==="

# Find LLVM
FOUND_LLVM=0

if command -v llvm-config &> /dev/null; then
    LLVM_PREFIX=$(llvm-config --prefix)
    LLVM_DIR="$LLVM_PREFIX/lib/cmake/llvm"
    if [ -f "$LLVM_DIR/LLVMConfig.cmake" ]; then
        echo "Found LLVM via llvm-config at: $LLVM_DIR"
        FOUND_LLVM=1
    fi
fi

# Try Homebrew opt link
if [ $FOUND_LLVM -eq 0 ] && [ -f "/opt/homebrew/opt/llvm/lib/cmake/llvm/LLVMConfig.cmake" ]; then
    LLVM_DIR="/opt/homebrew/opt/llvm/lib/cmake/llvm"
    echo "Found LLVM at: $LLVM_DIR"
    FOUND_LLVM=1
fi

# Try Homebrew Cellar
if [ $FOUND_LLVM -eq 0 ]; then
    LLVM_CELLAR_PATH=$(find /opt/homebrew/Cellar/llvm -name "LLVMConfig.cmake" 2>/dev/null | head -1)
    if [ -n "$LLVM_CELLAR_PATH" ]; then
        LLVM_DIR=$(dirname "$LLVM_CELLAR_PATH")
        echo "Found LLVM in Cellar at: $LLVM_DIR"
        FOUND_LLVM=1
    fi
fi

if [ $FOUND_LLVM -eq 0 ]; then
    echo "Error: Could not find LLVM"
    echo "Please install LLVM or run scripts/setup_llvm.sh"
    echo ""
    echo "For Homebrew on macOS:"
    echo "  brew install llvm"
    echo "  export PATH=\"/opt/homebrew/opt/llvm/bin:\$PATH\""
    exit 1
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring with CMake..."
cmake .. -DLLVM_DIR="$LLVM_DIR"

# Build
echo "Building..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "=== Build Complete ==="
echo "Binary location: $BUILD_DIR/llvm_dsl_compiler"

