#include "agents/VerificationAgent.h"
#include "utils/Logger.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>

bool VerificationAgent::verify(llvm::Module* module, bool printErrors) {
    LOG_INFO("VerificationAgent: Verifying module");
    
    std::string errors;
    llvm::raw_string_ostream OS(errors);
    
    bool isValid = llvm::verifyModule(*module, &OS);
    
    if (!isValid) {
        if (printErrors) {
            LOG_ERROR("VerificationAgent: Module verification failed:");
            LOG_ERROR(errors);
        }
        return false;
    }
    
    LOG_INFO("VerificationAgent: Module verification passed");
    return true;
}

std::string VerificationAgent::getVerificationErrors(llvm::Module* module) {
    std::string errors;
    llvm::raw_string_ostream OS(errors);
    llvm::verifyModule(*module, &OS);
    return errors;
}

