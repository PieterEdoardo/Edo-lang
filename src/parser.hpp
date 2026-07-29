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
    const Token& expect(TokenType type, const std::string& message);
    bool isAtEnd() const;

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
};