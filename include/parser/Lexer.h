#pragma once

#include <string>
#include <vector>
#include <fstream>

enum class TokenType {
    // Literals
    INT_LITERAL,
    FLOAT_LITERAL,
    BOOL_LITERAL,
    IDENTIFIER,
    
    // Keywords
    FN,
    LET,
    RETURN,
    IF,
    ELSE,
    WHILE,
    FOR,
    TRUE,
    FALSE,
    
    // Types
    I32,
    I64,
    F32,
    F64,
    BOOL,
    VOID,
    
    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    MOD,
    EQ,
    NE,
    LT,
    LE,
    GT,
    GE,
    AND,
    OR,
    NOT,
    ASSIGN,
    
    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COMMA,
    COLON,
    SEMICOLON,
    ARROW,
    
    // Special
    EOF_TOKEN,
    ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    
    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    Token scanNumber();
    Token scanIdentifier();
    Token scanString();
    
public:
    Lexer(const std::string& src);
    
    Token nextToken();
    std::vector<Token> tokenize();
};

