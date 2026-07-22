#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct NumberExpression;
struct IdentifierExpression;
struct BinaryExpression;

using Expression = std::variant<NumberExpression, IdentifierExpression, BinaryExpression>;
using ExpressionPointer = std::unique_ptr<Expression>;

struct NumberExpression {
    int value;
};

struct IdentifierExpression {
    std::string name;
};

struct BinaryExpression {
    ExpressionPointer left;
    std::string operatorSymbol;
    ExpressionPointer right;
};

struct AssignmentStatement;
struct BlockStatement;
struct IfStatement;

using Statement = std::variant<AssignmentStatement, BlockStatement, IfStatement>;
using StatementPointer = std::unique_ptr<Statement>;

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
};