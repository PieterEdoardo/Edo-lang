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
        case TokenType::KwMachine:      return "KwSyscall";
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
    const std::string source =
    "rax = 2 + 3 * 4 - 1;\n"
    "if (rax == 13) {\n"
    "rdi = 4;\n"
    "test(rax, rdi);\n"
    "} else if (rax == 0) {\n"
    "rdi = 6;\n"
    "} else {\n"
    "rdi = 7;\n"
    "}\n";
    // "print(rax, 1);\n";

    Lexer lexer(source);
    const std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);

    while (!parser.isAtEnd()) {
        StatementPointer statement = parser.parseStatement();
        std::cout << printStatement(*statement);
    }

    return 0;
}