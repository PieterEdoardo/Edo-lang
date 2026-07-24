#pragma once
#include <vector>
#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    ExpressionPointer parseExpression();

private:
    std::vector<Token> tokens;
    size_t position = 0;
    const Token& peek() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool isAtEnd() const;

    ExpressionPointer parsePrimary();
    ExpressionPointer parseAdditive();
};