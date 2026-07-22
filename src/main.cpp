#include <algorithm>
#include <iostream>
#include <string>
#include "lexer.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Number:      return "Number";
        case TokenType::Identifier:  return "Identifier";
        case TokenType::KwSyscall:   return "KwSyscall";
        case TokenType::KwIf:        return "KwIf";
        case TokenType::KwWhile:     return "KwWhile";
        case TokenType::LParen:      return "LParen";
        case TokenType::RParen:      return "RParen";
        case TokenType::LBrace:      return "LBrace";
        case TokenType::RBrace:      return "RBrace";
        case TokenType::Semicolon:   return "Semicolon";
        case TokenType::Equals:      return "Equals";
        case TokenType::Plus:        return "Plus";
        case TokenType::Minus:       return "Minus";
        case TokenType::EndOfFile:   return "EndOfFile";
        case TokenType::Invalid:     return "Invalid";
    }
    return "Unknown";
}

int main() {
    std:: string source = "rax = 5; \nif (rax) { rbx = rax + 1; }";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    for (const Token& token : tokens) {
        std::cout << "[" << token.line << "." << token.column << "] " << tokenTypeToString(token.type) << " \"" << token.lexeme << "\"\n";
    }

    return 0;
}