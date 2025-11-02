cmake_minimum_required(VERSION 3.20)
project(LLVM_DSL_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# LLVM components we need
llvm_map_components_to_libnames(llvm_libs
  Core
  ExecutionEngine
  OrcJIT
  OrcTargetProcess
  OrcShared
  IRReader
  Linker
  Passes
  Analysis
  TransformUtils
  InstCombine
  ScalarOpts
  Vectorize
  CodeGen
  Target
  TargetParser
  Support
  MC
  MCParser
  BitWriter
  BitReader
  AsmParser
  Object
  Option
  FrontendOpenMP
  LTO
  AArch64CodeGen
  AArch64AsmParser
  AArch64Desc
  AArch64Disassembler
  AArch64Info
  AArch64Utils
)

# Add executable
add_executable(llvm_dsl_compiler
  src/main.cpp
  src/agents/ParserAgent.cpp
  src/agents/ASTAgent.cpp
  src/agents/IRGenerationAgent.cpp
  src/agents/ModuleSetupAgent.cpp
  src/agents/OptimizationAgent.cpp
  src/agents/VerificationAgent.cpp
  src/agents/JITAgent.cpp
  src/agents/CodegenAgent.cpp
  src/agents/LinkerAgent.cpp
  src/agents/DiagnosticsAgent.cpp
  src/agents/SanitizerAgent.cpp
  src/ast/ASTNode.cpp
  src/ast/Expr.cpp
  src/ast/Stmt.cpp
  src/parser/Lexer.cpp
  src/parser/Parser.cpp
  src/utils/Logger.cpp
)

target_link_libraries(llvm_dsl_compiler ${llvm_libs})

# Enable sanitizers (optional, build with -DSANITIZERS=ON)
if(SANITIZERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

