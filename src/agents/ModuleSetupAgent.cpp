#include "agents/ModuleSetupAgent.h"
#include "utils/Logger.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <optional>

ModuleSetupAgent::ModuleSetupAgent() {
    // Initialize native target only (safe to call multiple times)
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    
    targetTriple = getDefaultTriple();
    dataLayout = getDefaultDataLayout(targetTriple);
}

std::string ModuleSetupAgent::getDefaultTriple() {
    return llvm::sys::getDefaultTargetTriple();
}

std::string ModuleSetupAgent::getDefaultDataLayout(const std::string& triple) {
    // Initialize target info
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(triple, error);
    if (!target) {
        LOG_WARNING("ModuleSetupAgent: Cannot find target for triple: " + triple);
        return ""; // Use default
    }
    
    llvm::TargetOptions opt;
    std::optional<llvm::Reloc::Model> RM = llvm::Reloc::Model::PIC_;
    llvm::Triple tripleObj(triple);
    auto targetMachine = std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(tripleObj, "generic", "", opt, RM));
    
    if (targetMachine) {
        return targetMachine->createDataLayout().getStringRepresentation();
    }
    
    LOG_WARNING("ModuleSetupAgent: Cannot create target machine for triple: " + triple);
    return "";
}

void ModuleSetupAgent::setupModule(llvm::Module* module) {
    LOG_INFO("ModuleSetupAgent: Setting up module");
    
    if (!targetTriple.empty()) {
        llvm::Triple triple(targetTriple);
        module->setTargetTriple(triple);
        LOG_INFO("ModuleSetupAgent: Target triple: " + targetTriple);
    }
    
    if (!dataLayout.empty()) {
        module->setDataLayout(dataLayout);
        LOG_INFO("ModuleSetupAgent: Data layout configured");
    } else {
        LOG_WARNING("ModuleSetupAgent: Using default data layout");
    }
}

void ModuleSetupAgent::setTargetTriple(const std::string& triple) {
    targetTriple = triple;
    dataLayout = getDefaultDataLayout(triple);
}

void ModuleSetupAgent::setDataLayout(const std::string& layout) {
    dataLayout = layout;
}

