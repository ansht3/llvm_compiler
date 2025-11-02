#include "agents/CodegenAgent.h"
#include "utils/Logger.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <optional>
#include <fstream>

CodegenAgent::CodegenAgent() {
    targetTriple = llvm::sys::getDefaultTargetTriple();
    // Initialize native target only (avoids linking all target backends)
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
}

bool CodegenAgent::initializeTarget() {
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        LOG_ERROR("CodegenAgent: Cannot find target: " + error);
        return false;
    }
    
    llvm::TargetOptions opt;
    std::optional<llvm::Reloc::Model> RM = std::nullopt;
    auto cpu = "generic";
    auto features = "";
    llvm::Triple tripleObj(targetTriple);
    
    targetMachine = std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(tripleObj, cpu, features, opt, RM));
    
    if (!targetMachine) {
        LOG_ERROR("CodegenAgent: Cannot create target machine");
        return false;
    }
    
    return true;
}

bool CodegenAgent::emitObjectFile(llvm::Module* module, const std::string& filename) {
    LOG_INFO("CodegenAgent: Emitting object file: " + filename);
    
    if (!initializeTarget()) {
        return false;
    }
    
    module->setDataLayout(targetMachine->createDataLayout());
    
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        LOG_ERROR("CodegenAgent: Cannot open file: " + EC.message());
        return false;
    }
    
    llvm::legacy::PassManager pass;
    llvm::CodeGenFileType fileType = llvm::CodeGenFileType::ObjectFile;
    
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        LOG_ERROR("CodegenAgent: TargetMachine cannot emit object file");
        return false;
    }
    
    pass.run(*module);
    dest.flush();
    
    LOG_INFO("CodegenAgent: Object file emitted successfully");
    return true;
}

bool CodegenAgent::emitAssemblyFile(llvm::Module* module, const std::string& filename) {
    LOG_INFO("CodegenAgent: Emitting assembly file: " + filename);
    
    if (!initializeTarget()) {
        return false;
    }
    
    module->setDataLayout(targetMachine->createDataLayout());
    
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        LOG_ERROR("CodegenAgent: Cannot open file: " + EC.message());
        return false;
    }
    
    llvm::legacy::PassManager pass;
    llvm::CodeGenFileType fileType = llvm::CodeGenFileType::AssemblyFile;
    
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        LOG_ERROR("CodegenAgent: TargetMachine cannot emit assembly file");
        return false;
    }
    
    pass.run(*module);
    dest.flush();
    
    LOG_INFO("CodegenAgent: Assembly file emitted successfully");
    return true;
}

bool CodegenAgent::emitBitcodeFile(llvm::Module* module, const std::string& filename) {
    LOG_INFO("CodegenAgent: Emitting bitcode file: " + filename);
    
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        LOG_ERROR("CodegenAgent: Cannot open file: " + EC.message());
        return false;
    }
    
    llvm::WriteBitcodeToFile(*module, dest);
    dest.flush();
    
    LOG_INFO("CodegenAgent: Bitcode file emitted successfully");
    return true;
}

bool CodegenAgent::emitIRFile(llvm::Module* module, const std::string& filename) {
    LOG_INFO("CodegenAgent: Emitting IR file: " + filename);
    
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        LOG_ERROR("CodegenAgent: Cannot open file: " + EC.message());
        return false;
    }
    
    module->print(dest, nullptr);
    dest.flush();
    
    LOG_INFO("CodegenAgent: IR file emitted successfully");
    return true;
}

bool CodegenAgent::emit(llvm::Module* module, const std::string& filename, OutputFormat format) {
    switch (format) {
        case OutputFormat::Object:
            return emitObjectFile(module, filename);
        case OutputFormat::Assembly:
            return emitAssemblyFile(module, filename);
        case OutputFormat::Bitcode:
            return emitBitcodeFile(module, filename);
        case OutputFormat::LLVM_IR:
            return emitIRFile(module, filename);
        default:
            LOG_ERROR("CodegenAgent: Unknown output format");
            return false;
    }
}

