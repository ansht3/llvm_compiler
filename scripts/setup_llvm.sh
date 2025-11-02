#!/bin/bash

# Script to help setup LLVM for the DSL compiler

echo "=== LLVM DSL Compiler Setup ==="

# Check if LLVM is installed
check_llvm() {
    if command -v llvm-config &> /dev/null; then
        echo "✓ LLVM found via llvm-config"
        llvm-config --version
        return 0
    fi
    
    # Check Homebrew LLVM installations (macOS)
    if [ -d "/opt/homebrew/opt/llvm" ]; then
        LLVM_BIN="/opt/homebrew/opt/llvm/bin/llvm-config"
        if [ -f "$LLVM_BIN" ]; then
            echo "✓ LLVM found at /opt/homebrew/opt/llvm"
            export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
            return 0
        fi
    fi
    
    # Check Homebrew Cellar (multiple versions)
    for llvm_path in /opt/homebrew/Cellar/llvm/*/bin/llvm-config; do
        if [ -f "$llvm_path" ]; then
            LLVM_DIR=$(dirname $(dirname "$llvm_path"))
            echo "✓ LLVM found at $LLVM_DIR"
            export PATH="$(dirname "$llvm_path"):$PATH"
            return 0
        fi
    done
    
    # Check common LLVM installation paths
    LLVM_PATHS=(
        "/usr/lib/llvm"
        "/usr/local/lib/llvm"
        "/opt/llvm"
    )
    
    for path in "${LLVM_PATHS[@]}"; do
        if [ -d "$path" ]; then
            echo "✓ LLVM found at $path"
            return 0
        fi
    done
    
    return 1
}

if ! check_llvm; then
    echo "✗ LLVM not found"
    echo ""
    echo "Please install LLVM:"
    echo ""
    echo "  macOS (Homebrew):"
    echo "    brew install llvm"
    echo ""
    echo "  Ubuntu/Debian:"
    echo "    sudo apt-get install llvm-dev libclang-dev"
    echo ""
    echo "  Fedora:"
    echo "    sudo dnf install llvm-devel clang-devel"
    echo ""
    echo "  From source:"
    echo "    https://llvm.org/docs/GettingStarted.html"
    exit 1
fi

# Find LLVM config
if command -v llvm-config &> /dev/null; then
    LLVM_VERSION=$(llvm-config --version)
    LLVM_PREFIX=$(llvm-config --prefix)
    LLVM_DIR="$LLVM_PREFIX/lib/cmake/llvm"
    
    # Verify CMake config exists
    if [ ! -f "$LLVM_DIR/LLVMConfig.cmake" ]; then
        # Try alternative location
        if [ -f "$LLVM_PREFIX/share/llvm/cmake/LLVMConfig.cmake" ]; then
            LLVM_DIR="$LLVM_PREFIX/share/llvm/cmake"
        elif [ -f "/opt/homebrew/Cellar/llvm/$LLVM_VERSION/lib/cmake/llvm/LLVMConfig.cmake" ]; then
            LLVM_DIR="/opt/homebrew/Cellar/llvm/$LLVM_VERSION/lib/cmake/llvm"
        fi
    fi
    
    echo ""
    echo "LLVM Configuration:"
    echo "  Version: $LLVM_VERSION"
    echo "  Prefix: $LLVM_PREFIX"
    echo "  CMake Dir: $LLVM_DIR"
    echo ""
    echo "To build the project, run:"
    echo "  mkdir build && cd build"
    echo "  cmake .. -DLLVM_DIR=$LLVM_DIR"
    echo "  make"
    echo ""
    echo "Or use the build script:"
    echo "  ./scripts/build.sh"
else
    # Try to find LLVM in Homebrew Cellar
    LLVM_CELLAR_PATH=$(find /opt/homebrew/Cellar/llvm -name "LLVMConfig.cmake" 2>/dev/null | head -1)
    if [ -n "$LLVM_CELLAR_PATH" ]; then
        LLVM_DIR=$(dirname "$LLVM_CELLAR_PATH")
        echo ""
        echo "Found LLVM CMake config at: $LLVM_DIR"
        echo ""
        echo "To build the project, run:"
        echo "  mkdir build && cd build"
        echo "  cmake .. -DLLVM_DIR=$LLVM_DIR"
        echo "  make"
        echo ""
        echo "Note: Add LLVM to your PATH for easier access:"
        echo "  export PATH=\"/opt/homebrew/opt/llvm/bin:\$PATH\""
    else
        echo "Warning: llvm-config not found"
        echo "You may need to manually specify LLVM_DIR when running cmake"
        echo ""
        echo "Try adding LLVM to your PATH:"
        echo "  export PATH=\"/opt/homebrew/opt/llvm/bin:\$PATH\""
        echo "  or"
        echo "  export PATH=\"/opt/homebrew/Cellar/llvm/*/bin:\$PATH\""
    fi
fi

echo ""
echo "=== Setup Complete ==="

