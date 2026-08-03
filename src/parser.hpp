#pragma once
#include <vector>
#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    [[nodiscard]] const Token& peek() const;
    const Token& advance();
    [[nodiscard]] bool check(TokenType type) const;

    static bool isTypeToken(TokenType type);

    const Token& expect(TokenType type, const std::string& message);
    bool isAtEnd() const;

    ExpressionPointer       parseExpression();
    ExpressionPointer       parsePrimary();

    // Grouped logic
    StatementPointer        parseAssignmentStatement();
    StatementPointer        parseBlockStatement();
    ExpressionPointer       parseComparison();

    // Calls
    ParameterDefinition     parseParameters();
    StatementPointer        parseFunctionDefinition();
    StatementPointer        parseMachineDefinition();

    // Statements
    StatementPointer        parseStatement();
    StatementPointer        parseIfStatement();
    StatementPointer        parseWhileStatement();
    StatementPointer        parseForStatement();
    StatementPointer        parseBreakStatement();
    StatementPointer        parseContinueStatement();
    StatementPointer        parseReturnStatement();

    // Operators
    ExpressionPointer       parseUnary();
    ExpressionPointer       parseAdditive();
    ExpressionPointer       parseMultiplicative();


    // Architecture
    StatementPointer        parseArchMap();

private:
    std::vector<Token> tokens; // internal
    size_t position = 0; // internal
};