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
struct WhileStatement;
struct ArchMap;

using Expression = std::variant<NumberExpression, IdentifierExpression, BinaryExpression, UnaryExpression>;
using ExpressionPointer = std::unique_ptr<Expression>;
using Statement = std::variant<AssignmentStatement, BlockStatement, IfStatement, WhileStatement, ArchMap>;
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

struct WhileStatement {
    ExpressionPointer condition;
    std::unique_ptr<BlockStatement> thenBranch;
};

// Architecture
struct RegisterMap {
    std::string identifier;
    std::string physicalRegisterName;
};

struct OpcodeMap {
    std::string identifier;
    std::string realOpcodeName;
};

struct ArchMap {
    std::string identifier;
    std::vector<RegisterMap> registers;
    std::vector<OpcodeMap> opcodes;
};