#include "parser.hpp"
#include <memory>
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

ExpressionPointer Parser::parseExpression() {
    return parseComparison();
}

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

const Token & Parser::expect(TokenType type, const std::string& expected) {
    if (check(type)) {
        return advance();
    }

    const Token& token = peek();
    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": expected '" + expected + ", got \"" + token.lexeme + "\""
    );
}


StatementPointer Parser::parseStatement() {
    // if (check(TokenType::KwSyscall)) {
    //     return parseSyscallStatement();
    // }

    if (check(TokenType::KwIf)) {
        return parseIfStatement();
    }

    if (check(TokenType::KwWhile)) {
        return parseWhileStatement();
    }

    // if (check(TokenType::KwFor)) {
    //     return parseForStatement();
    // }

    // if (check(TokenType::KwBreak)) {
    //     return parseBreakStatement();
    // }

    // if (check(TokenType::KwContinue)) {
    //     return parseContinueStatement();
    // }

    // if (check(TokenType::KwReturn)) {
    //     return parseReturnStatement();
    // }

    if (check(TokenType::LBrace)) {
        return parseBlockStatement();
    }

    return parseAssignmentStatement();
}

// Grouped logic
StatementPointer Parser::parseAssignmentStatement() {
    const Token& targetToken = expect(TokenType::Identifier, "identifier at start of assignment statement");
    const std::string targetName = targetToken.lexeme;

    expect(TokenType::Equals, "Expected '=' after identifier in assignment statement.");

    ExpressionPointer value = parseExpression();

    expect(TokenType::Semicolon, "Expected ';' after assignment statement.");

    return std::make_unique<Statement>(AssignmentStatement{targetName, std::move(value)});
}

StatementPointer Parser::parseBlockStatement() {
    expect(TokenType::LBrace, "Expected '{' after block statement.");

    std::vector<StatementPointer> statements;

    while (!check(TokenType::RBrace) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }

    expect(TokenType::RBrace, "Expected '}' at end of block statement.");

    return std::make_unique<Statement>(BlockStatement{std::move(statements)});
}

ExpressionPointer Parser::parseComparison() {
    ExpressionPointer left = parseAdditive();

    if (check(TokenType::EqualEqual) || check(TokenType::NotEqual) ||
        check(TokenType::Lesser) || check(TokenType::Greater) ||
        check(TokenType::LesserEqual) || check(TokenType::GreaterEqual)) {

        const Token& operatorToken = advance();
        ExpressionPointer right = parseAdditive();

        return std::make_unique<Expression>(BinaryExpression{
            .left = std::move(left),
            .operatorSymbol = operatorToken.lexeme,
            .right = std::move(right)
        });
    }

    return left;
}

// Statements
StatementPointer Parser::parseIfStatement() {
    expect(TokenType::KwIf, "'if' at start of if statement");
    expect(TokenType::LParen, "'(' after 'if'");

    ExpressionPointer condition = parseExpression();

    expect(TokenType::RParen, "')' after if condition");

    const StatementPointer thenStatement = parseBlockStatement();
    auto thenBranch = std::make_unique<BlockStatement>(
        std::move(std::get<BlockStatement>(*thenStatement))
    );

    std::unique_ptr<BlockStatement> elseBranch = nullptr;
    if (check(TokenType::KwElse)) {
        advance();
        const StatementPointer elseStatement = parseBlockStatement();
        elseBranch = std::make_unique<BlockStatement>(
            std::move(std::get<BlockStatement>(*elseStatement))
        );
    }

    return std::make_unique<Statement>(
        IfStatement{.condition = std::move(condition),
        .thenBranch = std::move(thenBranch),
        .elseBranch = std::move(elseBranch)}
    );
}

StatementPointer Parser::parseWhileStatement() {
    expect(TokenType::KwWhile, "'while' at start of while statement");
    expect(TokenType::LParen, "'(' after 'while'");

    ExpressionPointer condition = parseExpression();

    expect(TokenType::RParen, "')' after while condition");

    const StatementPointer thenStatement = parseBlockStatement();
    auto thenBranch = std::make_unique<BlockStatement>(
        std::move(std::get<BlockStatement>(*thenStatement))
    );

    return std::make_unique<Statement>(
        WhileStatement{
        .condition = std::move(condition),
        .thenBranch = std::move(thenBranch)
    });
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

// Operators
ExpressionPointer Parser::parseUnary() {
    if (check(TokenType::Not)) {
        const Token& operatorToken = advance();
        ExpressionPointer operand = parseUnary();

        return std::make_unique<Expression>(UnaryExpression{
            operatorToken.lexeme, std::move(operand)
        });
    }

    return parsePrimary();
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