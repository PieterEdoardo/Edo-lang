#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct NumberExpression;
struct IdentifierExpression;
struct BinaryExpression;
struct UnaryExpression;
struct AssignmentStatement;
struct BlockStatement;
struct IfStatement;

using Expression = std::variant<NumberExpression, IdentifierExpression, BinaryExpression, UnaryExpression>;
using ExpressionPointer = std::unique_ptr<Expression>;
using Statement = std::variant<AssignmentStatement, BlockStatement, IfStatement>;
using StatementPointer = std::unique_ptr<Statement>;

struct NumberExpression {
    int value;
};

struct UnaryExpression {
    std::string operatorSymbol;
    ExpressionPointer operand;
};

struct IdentifierExpression {
    std::string name;
};

struct BinaryExpression {
    ExpressionPointer left;
    std::string operatorSymbol;
    ExpressionPointer right;
};

struct AssignmentStatement {
    std::string target;
    ExpressionPointer value;
};

struct BlockStatement {
    std::vector<StatementPointer> statements;
};

struct IfStatement {
    ExpressionPointer condition;
    std::unique_ptr<BlockStatement> thenBranch;
    std::unique_ptr<BlockStatement> elseBranch;
};