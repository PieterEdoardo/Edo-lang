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
struct OpcodeMap;
struct RegisterMap;
struct ParameterDefinition;
struct TypeDefinition;
struct MachineDefinition;
struct FunctionDefinition;
struct CallExpression;
struct ExpressionStatement;
struct VariableDeclaration;

using Expression = std::variant<NumberExpression, IdentifierExpression, BinaryExpression, UnaryExpression, CallExpression>;
using ExpressionPointer = std::unique_ptr<Expression>;
using Statement = std::variant<AssignmentStatement, BlockStatement, IfStatement, WhileStatement, ArchMap, MachineDefinition, FunctionDefinition, ExpressionStatement, VariableDeclaration>;
using StatementPointer = std::unique_ptr<Statement>;

struct TypeDefinition {
    std::string name;
    bool isPointer;
    std::size_t byteSize;
};

struct VariableDeclaration {
    TypeDefinition type;
    std::string identifier;
    ExpressionPointer initializer;
};

struct ExpressionStatement {
    ExpressionPointer expression;
};

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

struct CallExpression {
    std::string identifier;
    std::vector<ExpressionPointer> arguments;
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
    StatementPointer elseBranch;
};

struct WhileStatement {
    ExpressionPointer condition;
    std::unique_ptr<BlockStatement> thenBranch;
};


struct ParameterDefinition {
    std::vector<TypeDefinition> types;
    std::vector<std::string> identifiers;
};

struct FunctionDefinition {
    TypeDefinition type;
    std::string identifier;
    ParameterDefinition parameters;
    std::unique_ptr<BlockStatement> block;
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

struct MachineDefinition {
    std::string identifier;
    ParameterDefinition parameters;
    std::unique_ptr<BlockStatement> block;
};