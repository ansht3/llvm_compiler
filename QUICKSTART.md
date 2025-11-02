# Quick Start Guide

## 5-Minute Setup

### 1. Install LLVM

**macOS:**
```bash
brew install llvm
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install llvm-dev libclang-dev
```

### 2. Build

```bash
./scripts/setup_llvm.sh    # Verify LLVM installation
./scripts/build.sh          # Build the compiler
```

### 3. Test

```bash
# Compile a sample program
./build/llvm_dsl_compiler examples/add.dsl --emit-ir -o add.ll

# Execute via JIT
./build/llvm_dsl_compiler examples/add.dsl --jit

# View generated IR
cat add.ll
```

## Common Commands

```bash
# Generate LLVM IR
./build/llvm_dsl_compiler input.dsl --emit-ir -o output.ll

# Generate object file
./build/llvm_dsl_compiler input.dsl --emit-obj -o output.o

# Execute immediately with JIT
./build/llvm_dsl_compiler input.dsl --jit

# Compile and link to executable
./build/llvm_dsl_compiler input.dsl --emit-obj -o output.o --link
./output.out

# Dump IR to console
./build/llvm_dsl_compiler input.dsl --dump-ir

# Optimize at different levels
./build/llvm_dsl_compiler input.dsl -O3 --emit-ir -o optimized.ll
```

## Sample Programs

See `examples/` directory:
- `add.dsl` - Simple addition
- `math.dsl` - Multiple operations
- `fib.dsl` - Recursive Fibonacci (requires if support)

## Architecture Overview

```
Source Code → ParserAgent → ASTAgent → IRGenerationAgent
                                    ↓
                            ModuleSetupAgent
                                    ↓
                            OptimizationAgent
                                    ↓
                            VerificationAgent
                                    ↓
                    ┌───────────────┴───────────────┐
                    ↓                               ↓
            JITAgent (execute)              CodegenAgent
                                              ↓
                                        LinkerAgent
```

## Next Steps

- Read [README.md](README.md) for detailed documentation
- Check [INSTALL.md](INSTALL.md) for troubleshooting
- Explore `examples/` for more sample programs
- Modify the DSL syntax in `src/parser/`

