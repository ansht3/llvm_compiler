#pragma once

#include "ast/Stmt.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>
#include <unordered_map>

class IRGenerationAgent {
private:
    llvm::LLVMContext& context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::unordered_map<std::string, llvm::Value*> namedValues;
    
    llvm::Value* codegenExpr(ast::Expr* expr);
    llvm::Value* codegenBinaryExpr(ast::BinaryExpr* expr);
    llvm::Value* codegenUnaryExpr(ast::UnaryExpr* expr);
    llvm::Value* codegenLiteral(ast::LiteralExpr* expr);
    llvm::Value* codegenVariable(ast::VariableExpr* expr);
    llvm::Value* codegenCall(ast::CallExpr* expr);
    
    void codegenStmt(ast::Stmt* stmt);
    void codegenReturn(ast::ReturnStmt* stmt);
    void codegenLet(ast::LetStmt* stmt);
    
    llvm::Function* codegenFunction(ast::Function* func);
    
public:
    IRGenerationAgent(llvm::LLVMContext& ctx);
    
    void generate(ast::Program* program);
    llvm::Module* getModule() { return module.get(); }
};

