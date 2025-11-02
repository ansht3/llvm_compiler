#pragma once

#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "ast/Stmt.h"
#include <string>
#include <memory>

class ParserAgent {
private:
    std::string source;
    
public:
    ParserAgent(const std::string& src) : source(src) {}
    
    std::unique_ptr<ast::Program> parse();
    
    static std::string readFile(const std::string& filename);
};

