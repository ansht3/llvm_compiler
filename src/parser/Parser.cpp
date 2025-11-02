#include "parser/Parser.h"
#include "utils/Logger.h"
#include <stdexcept>

Token Parser::peek() const {
    if (current >= tokens.size()) return tokens.back();
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    Token error = peek();
    LOG_ERROR(message + " at line " + std::to_string(error.line));
    throw std::runtime_error(message);
}

ast::Type Parser::parseType() {
    if (match(TokenType::I32)) return ast::Type(ast::Type::I32);
    if (match(TokenType::I64)) return ast::Type(ast::Type::I64);
    if (match(TokenType::F32)) return ast::Type(ast::Type::F32);
    if (match(TokenType::F64)) return ast::Type(ast::Type::F64);
    if (match(TokenType::BOOL)) return ast::Type(ast::Type::Bool);
    if (match(TokenType::VOID)) return ast::Type(ast::Type::Void);
    
    consume(TokenType::I32, "Expected type");
    return ast::Type(ast::Type::I32);
}

std::unique_ptr<ast::Expr> Parser::parsePrimary() {
    if (match(TokenType::INT_LITERAL)) {
        Token token = previous();
        return std::make_unique<ast::LiteralExpr>(
            token.value, ast::Type(ast::Type::I32), token.line, token.column);
    }
    
    if (match(TokenType::FLOAT_LITERAL)) {
        Token token = previous();
        return std::make_unique<ast::LiteralExpr>(
            token.value, ast::Type(ast::Type::F32), token.line, token.column);
    }
    
    if (match(TokenType::TRUE)) {
        Token token = previous();
        return std::make_unique<ast::LiteralExpr>(
            "1", ast::Type(ast::Type::Bool), token.line, token.column);
    }
    
    if (match(TokenType::FALSE)) {
        Token token = previous();
        return std::make_unique<ast::LiteralExpr>(
            "0", ast::Type(ast::Type::Bool), token.line, token.column);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        Token token = previous();
        std::string name = token.value;
        
        // Check if it's a function call
        if (check(TokenType::LPAREN)) {
            std::vector<std::unique_ptr<ast::Expr>> args;
            consume(TokenType::LPAREN, "Expected '(' after identifier");
            
            if (!check(TokenType::RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            
            consume(TokenType::RPAREN, "Expected ')' after arguments");
            return std::make_unique<ast::CallExpr>(name, std::move(args), token.line, token.column);
        }
        
        return std::make_unique<ast::VariableExpr>(name, token.line, token.column);
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    Token error = peek();
    LOG_ERROR("Expected expression at line " + std::to_string(error.line));
    throw std::runtime_error("Expected expression");
}

std::unique_ptr<ast::Expr> Parser::parseUnary() {
    if (match(TokenType::MINUS)) {
        Token op = previous();
        auto right = parseUnary();
        return std::make_unique<ast::UnaryExpr>(
            ast::UnaryOp::Neg, std::move(right), op.line, op.column);
    }
    
    if (match(TokenType::NOT)) {
        Token op = previous();
        auto right = parseUnary();
        return std::make_unique<ast::UnaryExpr>(
            ast::UnaryOp::Not, std::move(right), op.line, op.column);
    }
    
    return parsePrimary();
}

std::unique_ptr<ast::Expr> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MOD)) {
        Token op = previous();
        auto right = parseUnary();
        
        ast::BinaryOp binOp;
        if (op.type == TokenType::STAR) binOp = ast::BinaryOp::Mul;
        else if (op.type == TokenType::SLASH) binOp = ast::BinaryOp::Div;
        else binOp = ast::BinaryOp::Mod;
        
        expr = std::make_unique<ast::BinaryExpr>(
            std::move(expr), binOp, std::move(right), op.line, op.column);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = parseFactor();
        
        ast::BinaryOp binOp = (op.type == TokenType::PLUS) ? 
            ast::BinaryOp::Add : ast::BinaryOp::Sub;
        
        expr = std::make_unique<ast::BinaryExpr>(
            std::move(expr), binOp, std::move(right), op.line, op.column);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (match(TokenType::GT) || match(TokenType::GE) || 
           match(TokenType::LT) || match(TokenType::LE)) {
        Token op = previous();
        auto right = parseTerm();
        
        ast::BinaryOp binOp;
        if (op.type == TokenType::GT) binOp = ast::BinaryOp::Gt;
        else if (op.type == TokenType::GE) binOp = ast::BinaryOp::Ge;
        else if (op.type == TokenType::LT) binOp = ast::BinaryOp::Lt;
        else binOp = ast::BinaryOp::Le;
        
        expr = std::make_unique<ast::BinaryExpr>(
            std::move(expr), binOp, std::move(right), op.line, op.column);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(TokenType::EQ) || match(TokenType::NE)) {
        Token op = previous();
        auto right = parseComparison();
        
        ast::BinaryOp binOp = (op.type == TokenType::EQ) ? 
            ast::BinaryOp::Eq : ast::BinaryOp::Ne;
        
        expr = std::make_unique<ast::BinaryExpr>(
            std::move(expr), binOp, std::move(right), op.line, op.column);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<ast::ReturnStmt> Parser::parseReturn() {
    Token keyword = previous();
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after return");
    return std::make_unique<ast::ReturnStmt>(std::move(expr), keyword.line, keyword.column);
}

std::unique_ptr<ast::LetStmt> Parser::parseLet() {
    Token keyword = previous();
    consume(TokenType::IDENTIFIER, "Expected variable name");
    Token nameToken = previous();
    
    consume(TokenType::COLON, "Expected ':' after variable name");
    auto type = parseType();
    
    consume(TokenType::ASSIGN, "Expected '=' after type");
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after let statement");
    
    return std::make_unique<ast::LetStmt>(
        nameToken.value, type, std::move(value), keyword.line, keyword.column);
}

std::unique_ptr<ast::Stmt> Parser::parseStatement() {
    if (match(TokenType::RETURN)) {
        return parseReturn();
    }
    
    if (match(TokenType::LET)) {
        return parseLet();
    }
    
    // Expression statement (not fully implemented for simplicity)
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after statement");
    
    // For now, just return a placeholder
    return std::make_unique<ast::ReturnStmt>(std::move(expr), 0, 0);
}

std::unique_ptr<ast::Function> Parser::parseFunction() {
    consume(TokenType::IDENTIFIER, "Expected function name");
    Token nameToken = previous();
    
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    std::vector<std::pair<std::string, ast::Type>> params;
    if (!check(TokenType::RPAREN)) {
        do {
            consume(TokenType::IDENTIFIER, "Expected parameter name");
            Token paramName = previous();
            consume(TokenType::COLON, "Expected ':' after parameter name");
            auto paramType = parseType();
            params.push_back({paramName.value, paramType});
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    consume(TokenType::ARROW, "Expected '->' after parameters");
    auto returnType = parseType();
    
    consume(TokenType::LBRACE, "Expected '{' before function body");
    
    std::vector<std::unique_ptr<ast::Stmt>> body;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        body.push_back(parseStatement());
    }
    
    consume(TokenType::RBRACE, "Expected '}' after function body");
    
    return std::make_unique<ast::Function>(
        nameToken.value, returnType, std::move(params), std::move(body));
}

std::unique_ptr<ast::Program> Parser::parse() {
    auto program = std::make_unique<ast::Program>();
    
    while (!isAtEnd()) {
        if (match(TokenType::FN)) {
            program->addFunction(parseFunction());
        } else {
            consume(TokenType::FN, "Expected function declaration");
        }
    }
    
    return program;
}

