#pragma once

#include "ast/Stmt.h"
#include <memory>

class ASTAgent {
public:
    static void validateAST(ast::Program* program);
    static void dumpAST(ast::Program* program, int indent = 0);
};

