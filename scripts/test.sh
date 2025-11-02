#!/bin/bash

# Test script for LLVM DSL Compiler

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
COMPILER="$BUILD_DIR/llvm_dsl_compiler"

if [ ! -f "$COMPILER" ]; then
    echo "Error: Compiler not found. Please build first:"
    echo "  ./scripts/build.sh"
    exit 1
fi

echo "=== Testing LLVM DSL Compiler ==="

# Test with sample programs
TEST_FILES=(
    "$PROJECT_ROOT/examples/add.dsl"
    "$PROJECT_ROOT/examples/math.dsl"
)

PASSED=0
FAILED=0

for test_file in "${TEST_FILES[@]}"; do
    if [ ! -f "$test_file" ]; then
        echo "Warning: Test file not found: $test_file"
        continue
    fi
    
    echo ""
    echo "Testing: $test_file"
    
    # Compile to IR
    if "$COMPILER" "$test_file" --emit-ir -o "${test_file}.ll" 2>&1; then
        echo "  ✓ IR generation passed"
        
        # Verify IR
        if [ -f "${test_file}.ll" ]; then
            echo "  ✓ IR file created"
            
            # Try JIT execution
            if "$COMPILER" "$test_file" --jit 2>&1; then
                echo "  ✓ JIT execution passed"
                ((PASSED++))
            else
                echo "  ✗ JIT execution failed"
                ((FAILED++))
            fi
        else
            echo "  ✗ IR file not created"
            ((FAILED++))
        fi
    else
        echo "  ✗ Compilation failed"
        ((FAILED++))
    fi
done

echo ""
echo "=== Test Results ==="
echo "Passed: $PASSED"
echo "Failed: $FAILED"

if [ $FAILED -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed"
    exit 1
fi

