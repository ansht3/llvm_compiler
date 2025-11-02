#include "ast/ASTNode.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

namespace ast {

llvm::Type* Type::toLLVMType(llvm::LLVMContext& ctx) const {
    switch (kind) {
        case I32:
            return llvm::Type::getInt32Ty(ctx);
        case I64:
            return llvm::Type::getInt64Ty(ctx);
        case F32:
            return llvm::Type::getFloatTy(ctx);
        case F64:
            return llvm::Type::getDoubleTy(ctx);
        case Bool:
            return llvm::Type::getInt1Ty(ctx);
        case Void:
            return llvm::Type::getVoidTy(ctx);
        default:
            return llvm::Type::getVoidTy(ctx);
    }
}

Type Type::fromString(const std::string& name) {
    if (name == "i32") return Type(I32);
    if (name == "i64") return Type(I64);
    if (name == "f32") return Type(F32);
    if (name == "f64") return Type(F64);
    if (name == "bool") return Type(Bool);
    if (name == "void") return Type(Void);
    return Type(Void);
}

}

