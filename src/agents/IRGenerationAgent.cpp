#include "agents/IRGenerationAgent.h"
#include "utils/Logger.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

IRGenerationAgent::IRGenerationAgent(llvm::LLVMContext& ctx)
    : context(ctx), builder(std::make_unique<llvm::IRBuilder<>>(ctx)) {
    module = std::make_unique<llvm::Module>("DSL_Module", context);
    LOG_INFO("IRGenerationAgent: Initialized");
}

llvm::Value* IRGenerationAgent::codegenLiteral(ast::LiteralExpr* expr) {
    switch (expr->type.kind) {
        case ast::Type::I32: {
            int val = std::stoi(expr->value);
            return llvm::ConstantInt::get(context, llvm::APInt(32, val, true));
        }
        case ast::Type::I64: {
            long val = std::stol(expr->value);
            return llvm::ConstantInt::get(context, llvm::APInt(64, val, true));
        }
        case ast::Type::F32: {
            float val = std::stof(expr->value);
            return llvm::ConstantFP::get(context, llvm::APFloat(val));
        }
        case ast::Type::F64: {
            double val = std::stod(expr->value);
            return llvm::ConstantFP::get(context, llvm::APFloat(val));
        }
        case ast::Type::Bool: {
            bool val = (expr->value == "1" || expr->value == "true");
            return llvm::ConstantInt::get(context, llvm::APInt(1, val));
        }
        default:
            LOG_ERROR("IRGenerationAgent: Unsupported literal type");
            return nullptr;
    }
}

llvm::Value* IRGenerationAgent::codegenVariable(ast::VariableExpr* expr) {
    auto it = namedValues.find(expr->name);
    if (it == namedValues.end()) {
        LOG_ERROR("IRGenerationAgent: Unknown variable: " + expr->name);
        return nullptr;
    }
    return it->second;
}

llvm::Value* IRGenerationAgent::codegenCall(ast::CallExpr* expr) {
    llvm::Function* callee = module->getFunction(expr->callee);
    if (!callee) {
        LOG_ERROR("IRGenerationAgent: Unknown function: " + expr->callee);
        return nullptr;
    }
    
    if (callee->arg_size() != expr->args.size()) {
        LOG_ERROR("IRGenerationAgent: Argument count mismatch for: " + expr->callee);
        return nullptr;
    }
    
    std::vector<llvm::Value*> args;
    for (auto& arg : expr->args) {
        args.push_back(codegenExpr(arg.get()));
        if (!args.back()) return nullptr;
    }
    
    return builder->CreateCall(callee, args);
}

llvm::Value* IRGenerationAgent::codegenBinaryExpr(ast::BinaryExpr* expr) {
    llvm::Value* left = codegenExpr(expr->left.get());
    llvm::Value* right = codegenExpr(expr->right.get());
    
    if (!left || !right) return nullptr;
    
    switch (expr->op) {
        case ast::BinaryOp::Add:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFAdd(left, right, "addtmp");
            } else {
                return builder->CreateAdd(left, right, "addtmp");
            }
        case ast::BinaryOp::Sub:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFSub(left, right, "subtmp");
            } else {
                return builder->CreateSub(left, right, "subtmp");
            }
        case ast::BinaryOp::Mul:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFMul(left, right, "multmp");
            } else {
                return builder->CreateMul(left, right, "multmp");
            }
        case ast::BinaryOp::Div:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFDiv(left, right, "divtmp");
            } else {
                return builder->CreateSDiv(left, right, "divtmp");
            }
        case ast::BinaryOp::Mod:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFRem(left, right, "modtmp");
            } else {
                return builder->CreateSRem(left, right, "modtmp");
            }
        case ast::BinaryOp::Eq:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFCmpOEQ(left, right, "eqtmp");
            } else {
                return builder->CreateICmpEQ(left, right, "eqtmp");
            }
        case ast::BinaryOp::Ne:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFCmpONE(left, right, "netmp");
            } else {
                return builder->CreateICmpNE(left, right, "netmp");
            }
        case ast::BinaryOp::Lt:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFCmpOLT(left, right, "lttmp");
            } else {
                return builder->CreateICmpSLT(left, right, "lttmp");
            }
        case ast::BinaryOp::Le:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFCmpOLE(left, right, "letmp");
            } else {
                return builder->CreateICmpSLE(left, right, "letmp");
            }
        case ast::BinaryOp::Gt:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFCmpOGT(left, right, "gttmp");
            } else {
                return builder->CreateICmpSGT(left, right, "gttmp");
            }
        case ast::BinaryOp::Ge:
            if (left->getType()->isFloatingPointTy()) {
                return builder->CreateFCmpOGE(left, right, "getmp");
            } else {
                return builder->CreateICmpSGE(left, right, "getmp");
            }
        case ast::BinaryOp::And:
            return builder->CreateAnd(left, right, "andtmp");
        case ast::BinaryOp::Or:
            return builder->CreateOr(left, right, "ortmp");
        default:
            LOG_ERROR("IRGenerationAgent: Unsupported binary operator");
            return nullptr;
    }
}

llvm::Value* IRGenerationAgent::codegenUnaryExpr(ast::UnaryExpr* expr) {
    llvm::Value* operand = codegenExpr(expr->operand.get());
    if (!operand) return nullptr;
    
    switch (expr->op) {
        case ast::UnaryOp::Neg:
            if (operand->getType()->isFloatingPointTy()) {
                return builder->CreateFNeg(operand, "negtmp");
            } else {
                return builder->CreateNeg(operand, "negtmp");
            }
        case ast::UnaryOp::Not:
            return builder->CreateNot(operand, "nottmp");
        default:
            LOG_ERROR("IRGenerationAgent: Unsupported unary operator");
            return nullptr;
    }
}

llvm::Value* IRGenerationAgent::codegenExpr(ast::Expr* expr) {
    switch (expr->type) {
        case ast::ASTNodeType::Literal:
            return codegenLiteral(static_cast<ast::LiteralExpr*>(expr));
        case ast::ASTNodeType::Variable:
            return codegenVariable(static_cast<ast::VariableExpr*>(expr));
        case ast::ASTNodeType::BinaryExpr:
            return codegenBinaryExpr(static_cast<ast::BinaryExpr*>(expr));
        case ast::ASTNodeType::UnaryExpr:
            return codegenUnaryExpr(static_cast<ast::UnaryExpr*>(expr));
        case ast::ASTNodeType::Call:
            return codegenCall(static_cast<ast::CallExpr*>(expr));
        default:
            LOG_ERROR("IRGenerationAgent: Unsupported expression type");
            return nullptr;
    }
}

void IRGenerationAgent::codegenReturn(ast::ReturnStmt* stmt) {
    if (stmt->expr) {
        llvm::Value* retVal = codegenExpr(stmt->expr.get());
        builder->CreateRet(retVal);
    } else {
        builder->CreateRetVoid();
    }
}

void IRGenerationAgent::codegenLet(ast::LetStmt* stmt) {
    llvm::Value* val = codegenExpr(stmt->value.get());
    if (!val) {
        LOG_ERROR("IRGenerationAgent: Failed to generate code for let statement");
        return;
    }
    
    // Store in named values map
    namedValues[stmt->name] = val;
}

void IRGenerationAgent::codegenStmt(ast::Stmt* stmt) {
    switch (stmt->type) {
        case ast::ASTNodeType::Return:
            codegenReturn(static_cast<ast::ReturnStmt*>(stmt));
            break;
        case ast::ASTNodeType::Let:
            codegenLet(static_cast<ast::LetStmt*>(stmt));
            break;
        default:
            LOG_ERROR("IRGenerationAgent: Unsupported statement type");
    }
}

llvm::Function* IRGenerationAgent::codegenFunction(ast::Function* func) {
    // Check if function already exists
    llvm::Function* llvmFunc = module->getFunction(func->name);
    if (llvmFunc) {
        LOG_WARNING("IRGenerationAgent: Function already exists: " + func->name);
        return llvmFunc;
    }
    
    // Create function type
    std::vector<llvm::Type*> paramTypes;
    for (const auto& param : func->params) {
        paramTypes.push_back(param.second.toLLVMType(context));
    }
    
    llvm::Type* returnType = func->returnType.toLLVMType(context);
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    
    // Create function
    llvmFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, func->name, *module);
    
    // Set parameter names
    unsigned idx = 0;
    for (auto& arg : llvmFunc->args()) {
        arg.setName(func->params[idx].first);
        idx++;
    }
    
    // Create basic block
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", llvmFunc);
    builder->SetInsertPoint(bb);
    
    // Clear named values and add parameters
    namedValues.clear();
    for (auto& arg : llvmFunc->args()) {
        namedValues[std::string(arg.getName())] = &arg;
    }
    
    // Generate code for body
    for (auto& stmt : func->body) {
        codegenStmt(stmt.get());
    }
    
    // Add default return if needed
    if (!llvmFunc->getReturnType()->isVoidTy() && 
        !builder->GetInsertBlock()->getTerminator()) {
        LOG_ERROR("IRGenerationAgent: Function missing return statement");
        builder->CreateUnreachable();
    }
    
    return llvmFunc;
}

void IRGenerationAgent::generate(ast::Program* program) {
    LOG_INFO("IRGenerationAgent: Generating LLVM IR");
    
    for (const auto& func : program->functions) {
        codegenFunction(func.get());
    }
    
    LOG_INFO("IRGenerationAgent: IR generation completed");
}

