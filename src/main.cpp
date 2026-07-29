#include <algorithm>
#include <iostream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"

static std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Number:         return "Number";
        case TokenType::Identifier:     return "Identifier";
        case TokenType::KwSyscall:      return "KwSyscall";
        case TokenType::KwIf:           return "KwIf";
        case TokenType::KwWhile:        return "KwWhile";
        case TokenType::LParen:         return "LParen";
        case TokenType::RParen:         return "RParen";
        case TokenType::LBrace:         return "LBrace";
        case TokenType::RBrace:         return "RBrace";
        case TokenType::Semicolon:      return "Semicolon";
        case TokenType::Equals:         return "Equals";
        case TokenType::Plus:           return "Plus";
        case TokenType::Minus:          return "Minus";
        case TokenType::Star:           return "Star";
        case TokenType::Modulo:         return "Modulo";
        case TokenType::DQuote:         return "DQuote";
        case TokenType::SQuote:         return "SQuote";
        case TokenType::FSlash:         return "FSlash";
        case TokenType::BSlash:         return "BSlash";

        case TokenType::EndOfFile:      return "EndOfFile";
        case TokenType::Invalid:        return "Invalid";
        default:                        return "Unknown";
    }
}

int main() {
    std:: string source = "rax = 5; if (rax) { rbx = rax + 1; }";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    for (const Token& token : tokens) {
        std::cout << "[" << token.line << "." << token.column << "] " << tokenTypeToString(token.type) << " \"" << token.lexeme << "\"\n";
    }

    return 0;
}