# LLVM DSL Compiler - Multi-Agent Architecture

A complete Domain-Specific Language (DSL) compiler pipeline using LLVM with modular agent-based architecture. This project demonstrates production-quality compiler infrastructure with proper LLVM integration, optimization pipelines, and execution backends.

## Quick Start

```bash
# Build the compiler
./scripts/build.sh

# Compile a DSL program
./build/llvm_dsl_compiler examples/add.dsl --emit-obj -o add.o --link

# Run the executable
./add.out
```

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
./scripts/setup_llvm.sh
```

## Building

### Quick Setup

```bash
# Run setup script to find LLVM
./scripts/setup_llvm.sh

# Build the project
./scripts/build.sh
```

The binary will be created at: `build/llvm_dsl_compiler`

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

### Basic Usage (Like GCC)

The compiler works similarly to GCC/Clang:

```bash
# 1. Compile DSL source to object file
./build/llvm_dsl_compiler examples/add.dsl --emit-obj -o add.o

# 2. Link object file to executable
./build/llvm_dsl_compiler examples/add.dsl --emit-obj -o add.o --link
# This creates add.out (or you can specify -o add to name it)

# 3. Run the executable
./add.out
```

**One-liner (compile + link):**

```bash
./build/llvm_dsl_compiler examples/add.dsl --emit-obj -o add.o --link && ./add.out
```

### Other Output Formats

```bash
# Generate LLVM IR (human-readable intermediate code)
./build/llvm_dsl_compiler examples/add.dsl --emit-ir -o add.ll
cat add.ll

# Generate LLVM bitcode (binary IR)
./build/llvm_dsl_compiler examples/add.dsl --emit-bc -o add.bc

# Generate assembly code
./build/llvm_dsl_compiler examples/add.dsl --emit-asm -o add.s
cat add.s

# View IR in terminal
./build/llvm_dsl_compiler examples/add.dsl --dump-ir
```

### Execute Immediately (JIT)

```bash
# Compile and run in one step using ORC JIT
./build/llvm_dsl_compiler examples/add.dsl --jit
```

### Compiler Options

```bash
# Optimization levels (like -O0, -O1, -O2, -O3 in GCC)
./build/llvm_dsl_compiler examples/add.dsl -O3 --emit-obj -o add.o

# Disable optimizations
./build/llvm_dsl_compiler examples/add.dsl -O0 --emit-obj -o add.o

# Verbose output (see all agent operations)
./build/llvm_dsl_compiler examples/add.dsl -v --jit

# Enable sanitizers
./build/llvm_dsl_compiler examples/add.dsl --asan --emit-obj -o add.o
./build/llvm_dsl_compiler examples/add.dsl --ubsan --emit-obj -o add.o
```

### Full Command Reference

| Option       | Description                       | Example                    |
| ------------ | --------------------------------- | -------------------------- |
| `--emit-ir`  | Generate LLVM IR (.ll file)       | `--emit-ir -o output.ll`   |
| `--emit-obj` | Generate object file (.o file)    | `--emit-obj -o output.o`   |
| `--emit-bc`  | Generate bitcode (.bc file)       | `--emit-bc -o output.bc`   |
| `--emit-asm` | Generate assembly (.s file)       | `--emit-asm -o output.s`   |
| `--jit`      | Execute using ORC JIT             | `--jit`                    |
| `--dump-ir`  | Dump IR to stdout                 | `--dump-ir`                |
| `--link`     | Link object file to executable    | `--emit-obj -o x.o --link` |
| `-O<0-3>`    | Optimization level                | `-O2` (default)            |
| `-O0`        | Disable optimizations             | `-O0`                      |
| `--asan`     | Enable AddressSanitizer           | `--asan`                   |
| `--ubsan`    | Enable UndefinedBehaviorSanitizer | `--ubsan`                  |
| `-v`         | Verbose output                    | `-v`                       |
| `-o <file>`  | Output filename                   | `-o output.ll`             |

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
- `bool` - Boolean (`true`/`false`)
- `void` - Void type

### Operators

**Arithmetic:**

- `+` - Addition
- `-` - Subtraction
- `*` - Multiplication
- `/` - Division
- `%` - Modulo

**Comparison:**

- `==` - Equal
- `!=` - Not equal
- `<` - Less than
- `<=` - Less than or equal
- `>` - Greater than
- `>=` - Greater than or equal

**Logical:**

- `&&` - Logical AND
- `||` - Logical OR
- `!` - Logical NOT

### Syntax Rules

- Functions are declared with `fn name(params) -> return_type`
- Variables are declared with `let name: type = value;`
- All statements end with `;`
- Comments start with `//`

## Complete Workflow Example

```bash
# Step 1: Write a DSL program
cat > my_program.dsl << 'EOF'
fn multiply(a: i32, b: i32) -> i32 {
    return a * b;
}

fn main() -> i32 {
    let x: i32 = 10;
    let y: i32 = 5;
    return multiply(x, y);
}
EOF

# Step 2: Compile to object file
./build/llvm_dsl_compiler my_program.dsl --emit-obj -o my_program.o

# Step 3: Link to executable
./build/llvm_dsl_compiler my_program.dsl --emit-obj -o my_program.o --link

# Step 4: Run
./my_program.out

# Alternative: Use JIT for immediate execution
./build/llvm_dsl_compiler my_program.dsl --jit

# Alternative: View generated IR
./build/llvm_dsl_compiler my_program.dsl --dump-ir
```

## Example Programs

The `examples/` directory contains sample DSL programs:

```bash
# View examples
ls examples/

# Compile and run examples
./build/llvm_dsl_compiler examples/add.dsl --jit
./build/llvm_dsl_compiler examples/math.dsl --dump-ir
./build/llvm_dsl_compiler examples/math.dsl --emit-obj -o math.o --link
```

## Testing

```bash
# Run test suite
./scripts/test.sh
```

## How It Works

1. **ParserAgent** reads your `.dsl` file and tokenizes it
2. **ASTAgent** builds an Abstract Syntax Tree
3. **IRGenerationAgent** converts AST to LLVM IR (SSA form)
4. **ModuleSetupAgent** configures target architecture
5. **OptimizationAgent** applies optimizations (~O2 level)
6. **VerificationAgent** validates the IR
7. **CodegenAgent** generates object/assembly/bitcode
8. **LinkerAgent** links to create executable
9. **JITAgent** (optional) executes immediately

## Features

- ✅ Full LLVM IR generation with SSA form
- ✅ Proper target triple and data layout
- ✅ New Pass Manager optimizations (~O2 level)
- ✅ IR verification
- ✅ ORC JIT execution
- ✅ Object/bitcode/assembly emission
- ✅ Linking with lld/system linker
- ✅ Diagnostic reporting
- ✅ Sanitizer support (ASan/UBSan)
- ✅ Correct calling conventions and ABI
- ✅ LLVM intrinsics support (math/mem)
- ✅ Proper alignment and memory model

## Project Structure

```
llvm-test/
├── include/
│   ├── agents/      # 11 agent headers
│   ├── ast/         # AST node definitions
│   ├── parser/      # Lexer and parser
│   └── utils/       # Utilities
├── src/
│   ├── agents/      # 11 agent implementations
│   ├── ast/         # AST implementations
│   ├── parser/      # Parser implementations
│   ├── utils/       # Utility implementations
│   └── main.cpp     # Main entry point
├── examples/        # Sample DSL programs
│   ├── add.dsl      # Simple addition
│   ├── math.dsl     # Multiple operations
│   └── fib.dsl      # Recursive Fibonacci
├── scripts/         # Build and test scripts
│   ├── setup_llvm.sh
│   ├── build.sh
│   └── test.sh
└── CMakeLists.txt   # Build configuration
```

## Troubleshooting

### LLVM Not Found

```bash
# Run setup script
./scripts/setup_llvm.sh

# Or manually add to PATH (macOS Homebrew)
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
```

### Build Errors

```bash
# Clean and rebuild
rm -rf build
./scripts/build.sh
```

### Runtime Errors

- Make sure your DSL program has a `main()` function
- Check that all functions have return statements
- Verify syntax matches examples

## Comparison to GCC

| GCC Command                     | DSL Compiler Equivalent                                    |
| ------------------------------- | ---------------------------------------------------------- |
| `gcc -c file.c -o file.o`       | `./llvm_dsl_compiler file.dsl --emit-obj -o file.o`        |
| `gcc file.c -o file`            | `./llvm_dsl_compiler file.dsl --emit-obj -o file.o --link` |
| `gcc -O2 file.c`                | `./llvm_dsl_compiler file.dsl -O2 --emit-obj -o file.o`    |
| `gcc -S file.c`                 | `./llvm_dsl_compiler file.dsl --emit-asm -o file.s`        |
| `gcc -fsanitize=address file.c` | `./llvm_dsl_compiler file.dsl --asan --emit-obj -o file.o` |

## License

This is a demonstration project for educational purposes.

## Resources

- [LLVM Documentation](https://llvm.org/docs/)
- [LLVM IR Language Reference](https://llvm.org/docs/LangRef.html)
- [LLVM Tutorial](https://llvm.org/docs/tutorial/)
