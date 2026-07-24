#include "parser.hpp"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() const {
    return tokens[position];
}

const Token& Parser::advance() {
    const Token& token = tokens[position];

    if (!isAtEnd()) {
        position++;
    }

    return token;
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

ExpressionPointer Parser::parsePrimary() {
    const Token& token = peek();

    // Character groups
    if (check(TokenType::Number)) {
        advance();
        int value = std::stoi(token.lexeme);
        return std::make_unique<Expression>(NumberExpression{value});
    }

    if (check(TokenType::Identifier)) {
        advance();
        return std::make_unique<Expression>(IdentifierExpression{token.lexeme});
    }

    // Character specific
    if (check(TokenType::LParen)) {
        advance();
        ExpressionPointer inner = parseExpression();

        if (!check(TokenType::RParen)) {
            const Token& badToken = peek();
            throw std::runtime_error(
                "Parse error at line " + std::to_string(badToken.line) +
                ", column " + std::to_string(badToken.column) +
                ": expected ')' after expression"
            );
        }
        advance();

        return inner;
    }

    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": expected expression, got \"" + token.lexeme + "\""
    );
}

ExpressionPointer Parser::parseExpression() {
    return parseAdditive();
}

ExpressionPointer Parser::parseAdditive() {
    ExpressionPointer left = parsePrimary();

    while (check(TokenType::Plus) || check(TokenType::Minus)) {
        const Token& operatorToken = advance();
        ExpressionPointer right = parsePrimary();

        left = std::make_unique<Expression>(BinaryExpression{std::move(left), operatorToken.lexeme, std::move(right)});
    }

    return left;
}