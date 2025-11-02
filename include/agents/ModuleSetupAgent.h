#pragma once

#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Support/TargetSelect.h>
#include <memory>

class ModuleSetupAgent {
private:
    std::string targetTriple;
    std::string dataLayout;
    
public:
    ModuleSetupAgent();
    
    void setupModule(llvm::Module* module);
    void setTargetTriple(const std::string& triple);
    void setDataLayout(const std::string& layout);
    
    static std::string getDefaultTriple();
    static std::string getDefaultDataLayout(const std::string& triple);
};

