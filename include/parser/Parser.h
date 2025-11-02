#pragma once

#include "parser/Lexer.h"
#include "ast/Stmt.h"
#include <memory>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek() const;
    Token advance();
    Token previous() const;
    bool isAtEnd() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    
    std::unique_ptr<ast::Expr> parseExpression();
    std::unique_ptr<ast::Expr> parseEquality();
    std::unique_ptr<ast::Expr> parseComparison();
    std::unique_ptr<ast::Expr> parseTerm();
    std::unique_ptr<ast::Expr> parseFactor();
    std::unique_ptr<ast::Expr> parseUnary();
    std::unique_ptr<ast::Expr> parsePrimary();
    
    std::unique_ptr<ast::Stmt> parseStatement();
    std::unique_ptr<ast::ReturnStmt> parseReturn();
    std::unique_ptr<ast::LetStmt> parseLet();
    
    ast::Type parseType();
    std::unique_ptr<ast::Function> parseFunction();
    
public:
    Parser(const std::vector<Token>& toks) : tokens(toks), current(0) {}
    
    std::unique_ptr<ast::Program> parse();
};

