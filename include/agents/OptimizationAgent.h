#pragma once

#include <llvm/IR/Module.h>
#include <llvm/Passes/PassBuilder.h>
#include <memory>

class OptimizationAgent {
private:
    llvm::ModulePassManager modulePM;
    llvm::ModuleAnalysisManager moduleAM;
    llvm::CGSCCAnalysisManager cgsccAM;
    llvm::FunctionAnalysisManager functionAM;
    llvm::LoopAnalysisManager loopAM;
    
public:
    OptimizationAgent();
    
    void optimize(llvm::Module* module);
    void configureOptimizationLevel(int optLevel = 2);
};

