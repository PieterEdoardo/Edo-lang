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
    const Token& expect(TokenType type, const std::string& message);
    bool isAtEnd() const;

    ExpressionPointer parseExpression();
    ExpressionPointer parsePrimary();

    // Grouped logic
    StatementPointer parseAssignmentStatement();
    StatementPointer parseBlockStatement();
    ExpressionPointer parseComparison();

    // Statements
    StatementPointer parseStatement();
    StatementPointer parseSyscallStatement();
    StatementPointer parseIfStatement();
    StatementPointer parseElseStatement();
    StatementPointer parseWhileStatement();
    StatementPointer parseForStatement();
    StatementPointer parseBreakStatement();
    StatementPointer parseContinueStatement();
    StatementPointer parseReturnStatement();

    // Expressions
    ExpressionPointer parseUnary();
    ExpressionPointer parseAdditive();

private:
    std::vector<Token> tokens; // internal
    size_t position = 0; // internal
};