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

const Token & Parser::expect(TokenType type, const std::string &message) {
    if (check(type)) {
        return advance();
    }

    throw std::runtime_error(message);
}


StatementPointer Parser::parseStatement() {
    if (check(TokenType::KwSyscall)) {
        return parseSyscallStatement();
    }

    if (check(TokenType::KwIf)) {
        return parseIfStatement();
    }

    if (check(TokenType::KwElse)) {
        return parseElseStatement();
    }

    if (check(TokenType::KwWhile)) {
        return parseWhileStatement();
    }

    if (check(TokenType::KwFor)) {
        return parseForStatement();
    }

    if (check(TokenType::KwBreak)) {
        return parseBreakStatement();
    }

    if (check(TokenType::KwContinue)) {
        return parseContinueStatement();
    }

    if (check(TokenType::KwReturn)) {
        return parseReturnStatement();
    }

    if (check(TokenType::LBrace)) {
        return parseBlockStatement();
    }

    return parseAssignmentStatement();
}

StatementPointer Parser::parseAssignmentStatement() {
    const Token& token = expect(TokenType::Identifier, "Expected identifier at start of assignment statement.");

    expect(TokenType::Equals, "Expected '=' after identifier in assignment statement.");

    ExpressionPointer value = parseExpression();

    expect(TokenType::Semicolon, "Expected ';' after assignment statement.");

    AssignmentStatement assignment;
    assignment.target = token.lexeme;
    assignment.value = std::move(value);

    return std::make_unique<Statement>(std::move(assignment));
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