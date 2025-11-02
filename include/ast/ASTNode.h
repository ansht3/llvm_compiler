#pragma once

#include <string>
#include <vector>
#include <memory>
#include <llvm/IR/Type.h>

namespace ast {

enum class ASTNodeType {
    Program,
    Function,
    Variable,
    BinaryExpr,
    UnaryExpr,
    Literal,
    Return,
    Let,
    Call
};

enum class BinaryOp {
    Add, Sub, Mul, Div, Mod,
    Eq, Ne, Lt, Le, Gt, Ge,
    And, Or
};

enum class UnaryOp {
    Neg, Not
};

class ASTNode {
public:
    ASTNodeType type;
    int line;
    int column;
    
    ASTNode(ASTNodeType t, int l = 0, int c = 0) 
        : type(t), line(l), column(c) {}
    virtual ~ASTNode() = default;
};

class Type {
public:
    enum Kind {
        I32,
        I64,
        F32,
        F64,
        Bool,
        Void
    };
    
    Kind kind;
    
    Type(Kind k) : kind(k) {}
    
    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const;
    static Type fromString(const std::string& name);
};

} // namespace ast

