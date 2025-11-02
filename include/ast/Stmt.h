#pragma once

#include "ast/Expr.h"
#include <memory>
#include <vector>

namespace ast {

class Stmt : public ASTNode {
public:
    Stmt(ASTNodeType t, int l = 0, int c = 0) : ASTNode(t, l, c) {}
    virtual ~Stmt() = default;
};

class ReturnStmt : public Stmt {
public:
    std::unique_ptr<Expr> expr;
    
    ReturnStmt(std::unique_ptr<Expr> e, int line = 0, int col = 0)
        : Stmt(ASTNodeType::Return, line, col), expr(std::move(e)) {}
};

class LetStmt : public Stmt {
public:
    std::string name;
    Type type;
    std::unique_ptr<Expr> value;
    
    LetStmt(const std::string& n, Type t, std::unique_ptr<Expr> v, int line = 0, int col = 0)
        : Stmt(ASTNodeType::Let, line, col), name(n), type(t), value(std::move(v)) {}
};

class Function {
public:
    std::string name;
    Type returnType;
    std::vector<std::pair<std::string, Type>> params;
    std::vector<std::unique_ptr<Stmt>> body;
    
    Function(const std::string& n, Type rt, 
             std::vector<std::pair<std::string, Type>> p,
             std::vector<std::unique_ptr<Stmt>> b)
        : name(n), returnType(rt), params(std::move(p)), body(std::move(b)) {}
};

class Program {
public:
    std::vector<std::unique_ptr<Function>> functions;
    
    void addFunction(std::unique_ptr<Function> func) {
        functions.push_back(std::move(func));
    }
};

} // namespace ast

