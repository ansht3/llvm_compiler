#include "agents/ASTAgent.h"
#include "utils/Logger.h"
#include <iostream>
#include <stdexcept>

void ASTAgent::validateAST(ast::Program* program) {
    LOG_INFO("ASTAgent: Validating AST");
    
    if (!program) {
        LOG_ERROR("ASTAgent: Program is null");
        throw std::runtime_error("Invalid AST: program is null");
    }
    
    if (program->functions.empty()) {
        LOG_WARNING("ASTAgent: No functions in program");
    }
    
    LOG_INFO("ASTAgent: AST validation completed");
}

void ASTAgent::dumpAST(ast::Program* program, int indent) {
    // Simplified AST dumping
    for (const auto& func : program->functions) {
        std::cout << std::string(indent * 2, ' ') << "Function: " << func->name << std::endl;
        std::cout << std::string((indent + 1) * 2, ' ') << "Parameters: " << func->params.size() << std::endl;
        std::cout << std::string((indent + 1) * 2, ' ') << "Statements: " << func->body.size() << std::endl;
    }
}

