#pragma once

#include <llvm/IR/Module.h>

class SanitizerAgent {
public:
    static void addAddressSanitizer(llvm::Module* module);
    static void addUndefinedBehaviorSanitizer(llvm::Module* module);
    static void addSanitizers(llvm::Module* module, bool asan = false, bool ubsan = false);
};

