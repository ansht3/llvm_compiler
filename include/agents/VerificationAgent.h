#pragma once

#include <llvm/IR/Module.h>
#include <string>

class VerificationAgent {
public:
    static bool verify(llvm::Module* module, bool printErrors = true);
    static std::string getVerificationErrors(llvm::Module* module);
};

