# Installation Guide

## System Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.20 or later
- LLVM 11.0 or later
- Make or Ninja

## Step-by-Step Installation

### 1. Install LLVM

#### macOS

```bash
# Using Homebrew
brew install llvm

# Verify installation
llvm-config --version
```

#### Ubuntu/Debian

```bash
# For Ubuntu 20.04+
sudo apt-get update
sudo apt-get install llvm-14-dev libclang-14-dev

# For older versions
sudo apt-get install llvm-dev libclang-dev
```

#### Fedora/RHEL

```bash
sudo dnf install llvm-devel clang-devel
```

#### Building LLVM from Source

```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DLLVM_ENABLE_PROJECTS="clang;lld" \
      -G "Unix Makefiles" ../llvm
make -j$(nproc)
sudo make install
```

### 2. Clone/Setup Project

```bash
cd /Users/anshtandon/projects-embedded/llvm-test
```

### 3. Find LLVM Installation

```bash
./scripts/setup_llvm.sh
```

This script will:
- Check for LLVM installation
- Find the LLVM CMake directory
- Provide instructions for building

### 4. Build the Project

#### Using the build script (recommended):

```bash
./scripts/build.sh
```

#### Manual build:

```bash
mkdir build && cd build

# Find LLVM directory
LLVM_DIR=$(llvm-config --prefix)/lib/cmake/llvm

# Configure
cmake .. -DLLVM_DIR=$LLVM_DIR

# Build
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
```

### 5. Verify Installation

```bash
cd build
./llvm_dsl_compiler --help
```

### 6. Test

```bash
./scripts/test.sh
```

## Troubleshooting

### LLVM Not Found

**Problem:** `llvm-config: command not found`

**Solution:**
1. Ensure LLVM is installed: `brew install llvm` (macOS) or `apt-get install llvm-dev` (Linux)
2. Add LLVM to PATH:
   ```bash
   export PATH="/usr/local/opt/llvm/bin:$PATH"  # macOS Homebrew
   export PATH="/usr/lib/llvm-14/bin:$PATH"     # Ubuntu
   ```

### CMake Can't Find LLVM

**Problem:** `Could not find a package configuration file provided by "LLVM"`

**Solution:**
```bash
# Find LLVM manually
find /usr -name "LLVMConfig.cmake" 2>/dev/null
find /opt -name "LLVMConfig.cmake" 2>/dev/null

# Use found path in cmake
cmake .. -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm
```

### Compilation Errors

**Problem:** Missing LLVM headers or libraries

**Solution:**
```bash
# Verify LLVM installation
llvm-config --cxxflags
llvm-config --ldflags
llvm-config --libs

# If empty, reinstall LLVM
```

### Link Errors

**Problem:** Undefined references to LLVM symbols

**Solution:**
- Ensure all LLVM components are linked
- Check CMakeLists.txt includes all necessary LLVM libraries
- Verify LLVM was built with all required components

## Alternative: Using LLVM Docker Image

```bash
docker pull llvm/llvm:latest
docker run -it -v $(pwd):/workspace llvm/llvm:latest bash
cd /workspace
mkdir build && cd build
cmake ..
make
```

## Next Steps

After installation:
1. Read the main [README.md](README.md)
2. Try the example programs in `examples/`
3. Explore the agent architecture
4. Modify and extend the DSL syntax

