# now- Multi-Agent Architecture

A complete DSL compiler pipeline using LLVM with modular agent-based architecture. This project demonstrates a production-quality compiler infrastructure with proper LLVM integration, optimization pipelines, and execution backends.

## Architecture

The compiler is organized into 11 specialized agents:

1. **ParserAgent** - Lexical analysis and recursive descent parsing
2. **ASTAgent** - Abstract Syntax Tree representation and validation
3. **IRGenerationAgent** - AST to SSA LLVM IR conversion with proper memory model
4. **ModuleSetupAgent** - Target triple and data layout configuration
5. **OptimizationAgent** - New Pass Manager with ~O2 optimizations
6. **VerificationAgent** - IR validation and correctness checks
7. **JITAgent** - ORC JIT for native execution
8. **CodegenAgent** - Object/bitcode/assembly emission
9. **LinkerAgent** - Linking with lld or system linker
10. **DiagnosticsAgent** - Error reporting and IR dumping
11. **SanitizerAgent** - ASan/UBSan integration

## Prerequisites

### Installing LLVM

**macOS (Homebrew):**

```bash
brew install llvm
```

**Ubuntu/Debian:**

```bash
sudo apt-get install llvm-dev libclang-dev
```

**Fedora:**

```bash
sudo dnf install llvm-devel clang-devel
```

**From Source:**
See https://llvm.org/docs/GettingStarted.html

### Verify Installation

```bash
llvm-config --version
```

## Building

### Quick Setup

```bash
# Run setup script to find LLVM
./scripts/setup_llvm.sh

# Build the project
./scripts/build.sh
```

### Manual Build

```bash
mkdir build && cd build
cmake .. -DLLVM_DIR=$(llvm-config --prefix)/lib/cmake/llvm
make -j$(nproc)
```

### With Sanitizers

```bash
cmake .. -DSANITIZERS=ON -DLLVM_DIR=$(llvm-config --prefix)/lib/cmake/llvm
make
```

## Usage

### Basic Compilation

```bash
# Generate LLVM IR
./build/llvm_dsl_compiler examples/add.dsl --emit-ir -o add.ll

# Generate object file
./build/llvm_dsl_compiler examples/add.dsl --emit-obj -o add.o

# Generate bitcode
./build/llvm_dsl_compiler examples/add.dsl --emit-bc -o add.bc

# Generate assembly
./build/llvm_dsl_compiler examples/add.dsl --emit-asm -o add.s

# Execute via JIT
./build/llvm_dsl_compiler examples/add.dsl --jit

# Link to executable
./build/llvm_dsl_compiler examples/add.dsl --emit-obj -o add.o --link
```

### Options

- `--emit-ir` - Emit LLVM IR (.ll file)
- `--emit-obj` - Emit object file (.o file)
- `--emit-bc` - Emit bitcode (.bc file)
- `--emit-asm` - Emit assembly (.s file)
- `--jit` - Execute using ORC JIT
- `--dump-ir` - Dump IR to stdout
- `--link` - Link object file to executable
- `-O<level>` - Optimization level (0-3, default: 2)
- `-O0` - Disable optimizations
- `--asan` - Enable AddressSanitizer
- `--ubsan` - Enable UndefinedBehaviorSanitizer
- `-v` - Verbose output
- `-o <file>` - Output filename

## DSL Syntax

### Example Program

```dsl
// Simple addition function
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn main() -> i32 {
    let x: i32 = 42;
    let y: i32 = 58;
    return add(x, y);
}
```

### Supported Types

- `i32` - 32-bit signed integer
- `i64` - 64-bit signed integer
- `f32` - 32-bit float
- `f64` - 64-bit double
- `bool` - Boolean
- `void` - Void type

### Operators

- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `&&`, `||`, `!`

## Testing

```bash
./scripts/test.sh
```

## Features

- ✅ Full LLVM IR generation with SSA form
- ✅ Proper target triple and data layout
- ✅ New Pass Manager optimizations (~O2 level)
- ✅ IR verification
- ✅ ORC JIT execution
- ✅ Object/bitcode/assembly emission
- ✅ Linking with lld
- ✅ Diagnostic reporting
- ✅ Sanitizer support (ASan/UBSan)
- ✅ Correct calling conventions and ABI
- ✅ LLVM intrinsics support (math/mem)
- ✅ Proper alignment and memory model

## Project Structure

```
llvm-test/
├── include/
│   ├── agents/      # Agent headers
│   ├── ast/         # AST node definitions
│   ├── parser/      # Lexer and parser
│   └── utils/       # Utilities
├── src/
│   ├── agents/      # Agent implementations
│   ├── ast/         # AST implementations
│   ├── parser/      # Parser implementations
│   └── utils/       # Utility implementations
├── examples/        # Sample DSL programs
├── scripts/         # Build and test scripts
└── CMakeLists.txt   # Build configuration
```

## License

This is a demonstration project for educational purposes.
