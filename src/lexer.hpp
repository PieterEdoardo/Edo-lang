#pragma once
#include <string>
#include <vector>
#include "token.hpp"

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    const std::string source;
    size_t position = 0;
    int line = 1;
    int column = 1;

    char peek() const;
    char advance();
    bool isAtEnd() const;

    Token lexNumber();
    Token lexIdentifierOrKeyword();
    static Token makeToken(TokenType type, const std::string& lexeme, int tokenLine, int tokenColumn);
};