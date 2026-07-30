#include <algorithm>
#include <iostream>
#include <string>

#include "ast_printer.hpp"
#include "lexer.hpp"
#include "parser.hpp"

static std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Single
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
        case TokenType::Not:            return "Not";
        case TokenType::Lesser:         return "Lesser";
        case TokenType::Greater:        return "Greater";

        // Multiple
        case TokenType::EqualEqual:     return "EqualEqual";
        case TokenType::NotEqual:       return "NotEqual";
        case TokenType::LesserEqual:    return "LesserEqual";
        case TokenType::GreaterEqual:   return "GreaterEqual";

        case TokenType::EndOfFile:      return "EndOfFile";
        case TokenType::Invalid:        return "Invalid";
        default:                        return "Unknown";
    }
}

int main() {
    const std::string source = "rax = 5; if (rax == 5) { rbx = rax + 1; }";

    Lexer lexer(source);
    const std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);

    while (!parser.isAtEnd()) {
        StatementPointer statement = parser.parseStatement();
        std::cout << printStatement(*statement);
    }

    return 0;
}