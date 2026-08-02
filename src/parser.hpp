#pragma once
#include <vector>
#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    const Token& peek() const;
    const Token& advance();
    bool check(TokenType type) const;

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
    StatementPointer        parseFunctionCall();
    StatementPointer        parseMachineCall();

    // Statements
    StatementPointer        parseStatement();


    StatementPointer        parseIfStatement();
    StatementPointer        parseElseStatement();
    StatementPointer        parseWhileStatement();
    StatementPointer        parseForStatement();
    StatementPointer        parseBreakStatement();
    StatementPointer        parseContinueStatement();
    StatementPointer        parseReturnStatement();

    // Expressions
    ExpressionPointer       parseUnary();
    ExpressionPointer       parseAdditive();

    // Architecture
    StatementPointer        parseArchMap();

private:
    std::vector<Token> tokens; // internal
    size_t position = 0; // internal
};