#pragma once

#include "ast/ASTNode.h"
#include <memory>

namespace ast {

class Expr : public ASTNode {
public:
    Expr(ASTNodeType t, int l = 0, int c = 0) : ASTNode(t, l, c) {}
    virtual ~Expr() = default;
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    BinaryOp op;
    
    BinaryExpr(std::unique_ptr<Expr> l, BinaryOp o, std::unique_ptr<Expr> r, int line = 0, int col = 0)
        : Expr(ASTNodeType::BinaryExpr, line, col), left(std::move(l)), op(o), right(std::move(r)) {}
};

class UnaryExpr : public Expr {
public:
    std::unique_ptr<Expr> operand;
    UnaryOp op;
    
    UnaryExpr(UnaryOp o, std::unique_ptr<Expr> operand, int line = 0, int col = 0)
        : Expr(ASTNodeType::UnaryExpr, line, col), op(o), operand(std::move(operand)) {}
};

class LiteralExpr : public Expr {
public:
    std::string value;
    Type type;
    
    LiteralExpr(const std::string& v, Type t, int line = 0, int col = 0)
        : Expr(ASTNodeType::Literal, line, col), value(v), type(t) {}
};

class VariableExpr : public Expr {
public:
    std::string name;
    
    VariableExpr(const std::string& n, int line = 0, int col = 0)
        : Expr(ASTNodeType::Variable, line, col), name(n) {}
};

class CallExpr : public Expr {
public:
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    
    CallExpr(const std::string& c, std::vector<std::unique_ptr<Expr>> a, int line = 0, int col = 0)
        : Expr(ASTNodeType::Call, line, col), callee(c), args(std::move(a)) {}
};

} // namespace ast

