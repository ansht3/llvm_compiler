#pragma once

#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <memory>
#include <string>

class JITAgent {
private:
    std::unique_ptr<llvm::orc::LLJIT> jit;
    llvm::LLVMContext context;
    
public:
    JITAgent();
    ~JITAgent();
    
    bool initialize();
    bool addModule(std::unique_ptr<llvm::Module> module);
    void* getFunctionAddress(const std::string& name);
    
    template<typename Func>
    Func getFunction(const std::string& name) {
        void* addr = getFunctionAddress(name);
        return reinterpret_cast<Func>(addr);
    }
};

