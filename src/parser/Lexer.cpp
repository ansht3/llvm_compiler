#include "parser/Lexer.h"
#include <cctype>
#include <sstream>
#include "utils/Logger.h"

Lexer::Lexer(const std::string& src) 
    : source(src), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (pos >= source.length()) return '\0';
    char c = source[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (pos < source.length()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
        while (pos < source.length() && peek() != '\n') {
            advance();
        }
    }
}

Token Lexer::scanNumber() {
    int startLine = line;
    int startCol = column;
    std::string num;
    bool isFloat = false;
    
    while (pos < source.length()) {
        char c = peek();
        if (std::isdigit(c)) {
            num += advance();
        } else if (c == '.' && !isFloat) {
            isFloat = true;
            num += advance();
        } else {
            break;
        }
    }
    
    return Token(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL,
                 num, startLine, startCol);
}

Token Lexer::scanIdentifier() {
    int startLine = line;
    int startCol = column;
    std::string ident;
    
    while (pos < source.length()) {
        char c = peek();
        if (std::isalnum(c) || c == '_') {
            ident += advance();
        } else {
            break;
        }
    }
    
    // Check for keywords
    if (ident == "fn") return Token(TokenType::FN, ident, startLine, startCol);
    if (ident == "let") return Token(TokenType::LET, ident, startLine, startCol);
    if (ident == "return") return Token(TokenType::RETURN, ident, startLine, startCol);
    if (ident == "if") return Token(TokenType::IF, ident, startLine, startCol);
    if (ident == "else") return Token(TokenType::ELSE, ident, startLine, startCol);
    if (ident == "while") return Token(TokenType::WHILE, ident, startLine, startCol);
    if (ident == "for") return Token(TokenType::FOR, ident, startLine, startCol);
    if (ident == "true") return Token(TokenType::TRUE, ident, startLine, startCol);
    if (ident == "false") return Token(TokenType::FALSE, ident, startLine, startCol);
    if (ident == "i32") return Token(TokenType::I32, ident, startLine, startCol);
    if (ident == "i64") return Token(TokenType::I64, ident, startLine, startCol);
    if (ident == "f32") return Token(TokenType::F32, ident, startLine, startCol);
    if (ident == "f64") return Token(TokenType::F64, ident, startLine, startCol);
    if (ident == "bool") return Token(TokenType::BOOL, ident, startLine, startCol);
    if (ident == "void") return Token(TokenType::VOID, ident, startLine, startCol);
    
    return Token(TokenType::IDENTIFIER, ident, startLine, startCol);
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (pos >= source.length()) {
        return Token(TokenType::EOF_TOKEN, "", line, column);
    }
    
    // Skip comments
    if (peek() == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
        skipComment();
        return nextToken();
    }
    
    char c = peek();
    int startLine = line;
    int startCol = column;
    
    // Single character tokens
    switch (c) {
        case '(':
            advance();
            return Token(TokenType::LPAREN, "(", startLine, startCol);
        case ')':
            advance();
            return Token(TokenType::RPAREN, ")", startLine, startCol);
        case '{':
            advance();
            return Token(TokenType::LBRACE, "{", startLine, startCol);
        case '}':
            advance();
            return Token(TokenType::RBRACE, "}", startLine, startCol);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", startLine, startCol);
        case ':':
            advance();
            return Token(TokenType::COLON, ":", startLine, startCol);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", startLine, startCol);
        case '+':
            advance();
            return Token(TokenType::PLUS, "+", startLine, startCol);
        case '-':
            advance();
            if (peek() == '>') {
                advance();
                return Token(TokenType::ARROW, "->", startLine, startCol);
            }
            return Token(TokenType::MINUS, "-", startLine, startCol);
        case '*':
            advance();
            return Token(TokenType::STAR, "*", startLine, startCol);
        case '/':
            advance();
            return Token(TokenType::SLASH, "/", startLine, startCol);
        case '%':
            advance();
            return Token(TokenType::MOD, "%", startLine, startCol);
        case '=':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQ, "==", startLine, startCol);
            }
            return Token(TokenType::ASSIGN, "=", startLine, startCol);
        case '!':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::NE, "!=", startLine, startCol);
            }
            return Token(TokenType::NOT, "!", startLine, startCol);
        case '<':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::LE, "<=", startLine, startCol);
            }
            return Token(TokenType::LT, "<", startLine, startCol);
        case '>':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::GE, ">=", startLine, startCol);
            }
            return Token(TokenType::GT, ">", startLine, startCol);
        case '&':
            advance();
            if (peek() == '&') {
                advance();
                return Token(TokenType::AND, "&&", startLine, startCol);
            }
            return Token(TokenType::ERROR, "&", startLine, startCol);
        case '|':
            advance();
            if (peek() == '|') {
                advance();
                return Token(TokenType::OR, "||", startLine, startCol);
            }
            return Token(TokenType::ERROR, "|", startLine, startCol);
    }
    
    // Numbers
    if (std::isdigit(c)) {
        return scanNumber();
    }
    
    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // Unknown character
    std::string error = "Unexpected character: ";
    error += c;
    LOG_ERROR(error);
    advance();
    return Token(TokenType::ERROR, error, startLine, startCol);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token = nextToken();
    while (token.type != TokenType::EOF_TOKEN) {
        tokens.push_back(token);
        token = nextToken();
    }
    tokens.push_back(token); // Add EOF
    return tokens;
}

