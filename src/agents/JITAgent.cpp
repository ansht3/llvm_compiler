#include "agents/JITAgent.h"
#include "utils/Logger.h"
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/Support/Error.h>

JITAgent::JITAgent() {
}

JITAgent::~JITAgent() {
}

bool JITAgent::initialize() {
    LOG_INFO("JITAgent: Initializing ORC JIT");
    
    auto jitOrError = llvm::orc::LLJITBuilder().create();
    if (!jitOrError) {
        LOG_ERROR("JITAgent: Failed to create JIT instance");
        llvm::logAllUnhandledErrors(jitOrError.takeError(), llvm::errs(), "JIT Error: ");
        return false;
    }
    
    jit = std::move(*jitOrError);
    LOG_INFO("JITAgent: ORC JIT initialized successfully");
    return true;
}

bool JITAgent::addModule(std::unique_ptr<llvm::Module> module) {
    if (!jit) {
        LOG_ERROR("JITAgent: JIT not initialized");
        return false;
    }
    
    LOG_INFO("JITAgent: Adding module to JIT");
    
    llvm::orc::ThreadSafeModule tsm(std::move(module), 
                                      std::make_unique<llvm::LLVMContext>());
    
    auto err = jit->addIRModule(std::move(tsm));
    if (err) {
        LOG_ERROR("JITAgent: Failed to add module");
        llvm::logAllUnhandledErrors(std::move(err), llvm::errs(), "JIT Error: ");
        return false;
    }
    
    LOG_INFO("JITAgent: Module added successfully");
    return true;
}

void* JITAgent::getFunctionAddress(const std::string& name) {
    if (!jit) {
        LOG_ERROR("JITAgent: JIT not initialized");
        return nullptr;
    }
    
    auto sym = jit->lookup(name);
    if (!sym) {
        LOG_ERROR("JITAgent: Function not found: " + name);
        llvm::logAllUnhandledErrors(sym.takeError(), llvm::errs(), "JIT Error: ");
        return nullptr;
    }
    
    return reinterpret_cast<void*>(sym->getValue());
}

