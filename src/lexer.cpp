#include "lexer.hpp"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(std::string source) : source(std::move(source)) {}

bool Lexer::isAtEnd() const {
    return position >= source.size();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::advance() {
    char c = source[position++];

    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    return c;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        char c = peek();

        if (std::isspace(static_cast<unsigned char>(c))) {
            advance();
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(lexNumber());
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
            continue;
        }

        int startLine = line;
        int startColumn = column;
        advance();

        switch (c) {
            case '(': tokens.push_back(makeToken(TokenType::LParen, "(")); break;
            case ')': tokens.push_back(makeToken(TokenType::RParen, ")")); break;
            case '{': tokens.push_back(makeToken(TokenType::LBrace, "{")); break;
            case '}': tokens.push_back(makeToken(TokenType::RBrace, "}")); break;
            case '[': tokens.push_back(makeToken(TokenType::LBracket, "[")); break;
            case ']': tokens.push_back(makeToken(TokenType::RBracket, "]")); break;
            case ';': tokens.push_back(makeToken(TokenType::Semicolon, ";")); break;
            case '=': tokens.push_back(makeToken(TokenType::Equals, "=")); break;
            case '+': tokens.push_back(makeToken(TokenType::Plus, "+")); break;
            case '-': tokens.push_back(makeToken(TokenType::Minus, "-")); break;
            case '*': tokens.push_back(makeToken(TokenType::Star, "*")); break;
            case '\\': tokens.push_back(makeToken(TokenType::Slash, "\\")); break;
            default: {
                Token invalid{TokenType::Invalid, std::string(1, c), startLine, startColumn};
                tokens.push_back(invalid);
            }
        }
    }

    tokens.push_back(Token{TokenType::EndOfFile, "", line, column});
    return tokens;
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) const {
    return Token{type, lexeme, line, column};
}

Token Lexer::lexNumber() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;

    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    std::string lexeme = source.substr(start, position - start);

    return Token{TokenType::Number, lexeme, startLine, startColumn};
}