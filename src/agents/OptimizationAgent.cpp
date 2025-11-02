#include "agents/OptimizationAgent.h"
#include "utils/Logger.h"
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/Utils.h>

OptimizationAgent::OptimizationAgent() {
    configureOptimizationLevel(2);
}

void OptimizationAgent::configureOptimizationLevel(int optLevel) {
    LOG_INFO("OptimizationAgent: Configuring optimization level " + std::to_string(optLevel));
    
    llvm::PassBuilder passBuilder;
    
    passBuilder.registerModuleAnalyses(moduleAM);
    passBuilder.registerCGSCCAnalyses(cgsccAM);
    passBuilder.registerFunctionAnalyses(functionAM);
    passBuilder.registerLoopAnalyses(loopAM);
    passBuilder.crossRegisterProxies(loopAM, functionAM, cgsccAM, moduleAM);
    
    // Build optimization pipeline (~O2)
    llvm::ModulePassManager mpm;
    
    llvm::PipelineTuningOptions PTO;
    llvm::ThinOrFullLTOPhase LTOPhase = llvm::ThinOrFullLTOPhase::None;
    
    llvm::OptimizationLevel optLevelEnum;
    switch(optLevel) {
        case 0: optLevelEnum = llvm::OptimizationLevel::O0; break;
        case 1: optLevelEnum = llvm::OptimizationLevel::O1; break;
        case 2: optLevelEnum = llvm::OptimizationLevel::O2; break;
        case 3: optLevelEnum = llvm::OptimizationLevel::O3; break;
        default: optLevelEnum = llvm::OptimizationLevel::O2; break;
    }
    
    if (optLevel >= 1) {
        // Level 1+ optimizations
        mpm.addPass(llvm::createModuleToFunctionPassAdaptor(
            passBuilder.buildFunctionSimplificationPipeline(
                optLevelEnum, LTOPhase)));
        
        // Additional module-level passes for O2+
        if (optLevel >= 2) {
            mpm.addPass(passBuilder.buildModuleSimplificationPipeline(
                optLevelEnum, LTOPhase));
        }
    }
    
    modulePM = std::move(mpm);
}

void OptimizationAgent::optimize(llvm::Module* module) {
    LOG_INFO("OptimizationAgent: Running optimizations");
    modulePM.run(*module, moduleAM);
    LOG_INFO("OptimizationAgent: Optimizations completed");
}

