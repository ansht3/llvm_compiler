#include "agents/ParserAgent.h"
#include "utils/Logger.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::unique_ptr<ast::Program> ParserAgent::parse() {
    LOG_INFO("ParserAgent: Starting lexical analysis");
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    LOG_INFO("ParserAgent: Starting parsing");
    Parser parser(tokens);
    auto program = parser.parse();
    
    LOG_INFO("ParserAgent: Parsing completed");
    return program;
}

std::string ParserAgent::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("ParserAgent: Cannot open file: " + filename);
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

