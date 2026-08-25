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
    [[nodiscard]] bool checkAhead(TokenType type, std::size_t offset) const;

    bool checkAny(std::initializer_list<TokenType> types) const;

    static bool isTypeToken(TokenType type);

    static std::size_t byteSizeForType(TokenType type);

    const Token& expect(TokenType type, const std::string& message);
    [[nodiscard]] bool isAtEnd() const;

    // Statements
    StatementPointer        parseStatement();
    StatementPointer        parseCallStatement();
    StatementPointer        parseStructDefinition();
    StatementPointer        parseIfStatement();
    StatementPointer        parseWhileStatement();
    StatementPointer        parseBreakStatement();
    StatementPointer        parseContinueStatement();
    StatementPointer        parseReturnStatement();
    StatementPointer        parseArchMap();
    StatementPointer        parseFunctionDefinition();
    StatementPointer        parseMachineDefinition();
    StatementPointer        parseAssignmentStatement();
    StatementPointer        parseBlockStatement();
    StatementPointer        parseVariableDeclaration();

    // Expressions
    ExpressionPointer       parseExpression();
    ExpressionPointer       parsePrimary();
    ExpressionPointer       parseUnary();
    ExpressionPointer       parseAdditive();
    ExpressionPointer       parseMultiplicative();
    ExpressionPointer       parseComparison();

    // Grouped logic
    TypeDefinition          parseTypeDefinition(bool useRegisters);
    std::vector<ExpressionPointer> parseArguments();
    ParameterDefinition     parseParameters(bool useRegisters);

private:
    std::vector<Token> tokens; // internal
    std::size_t position = 0; // internal
};