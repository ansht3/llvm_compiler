#pragma once

#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>

enum class OutputFormat {
    Object,
    Assembly,
    Bitcode,
    LLVM_IR
};

class CodegenAgent {
private:
    std::string targetTriple;
    std::unique_ptr<llvm::TargetMachine> targetMachine;
    
    bool initializeTarget();
    
public:
    CodegenAgent();
    
    bool emitObjectFile(llvm::Module* module, const std::string& filename);
    bool emitAssemblyFile(llvm::Module* module, const std::string& filename);
    bool emitBitcodeFile(llvm::Module* module, const std::string& filename);
    bool emitIRFile(llvm::Module* module, const std::string& filename);
    
    bool emit(llvm::Module* module, const std::string& filename, OutputFormat format);
};

