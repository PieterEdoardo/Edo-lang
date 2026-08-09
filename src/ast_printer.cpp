#include "ast_printer.hpp"
#include <variant>

namespace {
    template<class... Ts>
    struct Overloaded : Ts... {
        using Ts::operator()...;
    };
    template<class... Ts>
    Overloaded(Ts...) -> Overloaded<Ts...>;
}

static std::string printBlockStatement(const BlockStatement& block, const int indent) {
    const std::string pad(indent * 2, ' ');
    std::string result = pad + "{\n";
    for (const auto& inner : block.statements) {
        result += printStatement(*inner, indent + 1);
    }
    result += pad + "}\n";
    return result;
}

static std::string printParameters(const ParameterDefinition& parameter) {
    std::string result = "(";
    for (std::size_t i = 0; i < parameter.identifiers.size(); ++i) {
        result += parameter.types[i].name + " " + parameter.identifiers[i];
        result += (i + 1 == parameter.identifiers.size()) ? ") " : ", ";
    }
    return result;
}

std::string printExpression(Expression& expression) {
    return std::visit(Overloaded{
        [](const NumberExpression &number) -> std::string {
            return std::to_string(number.value);
        },
        [](const IdentifierExpression &identifier) -> std::string {
            return identifier.name;
        },
        [](const BinaryExpression &binary) -> std::string {
            return "(" + printExpression(*binary.left) + " " +
                binary.operatorSymbol + " " +
                printExpression(*binary.right) + ")";
        },
        [](const UnaryExpression &unary) -> std::string {
            return "(" + unary.operatorSymbol + " " + printExpression(*unary.operand) + ")";
        },
        [](const CallExpression &call) -> std::string {
            return "";
        }
    }, expression);
}


std::string printStatement(Statement& statement, const int indent) {
    const std::string pad(indent * 2, ' ');

    return std::visit(Overloaded{
        [&](const AssignmentStatement &assignment) -> std::string {
            return pad + assignment.target + " = " + printExpression(*assignment.value) + ";\n";
        },
        [&](const BlockStatement &block) -> std::string {
            return printBlockStatement(block, indent);
        },
        [&](const IfStatement &ifStatement) -> std::string {
            std::string result = pad + "if (" + printExpression(*ifStatement.condition) + ")\n";
            result += printBlockStatement(*ifStatement.thenBranch, indent);
            if (ifStatement.elseBranch) {
                result += pad + "else\n";
                result += printStatement(*ifStatement.elseBranch, indent);
            }
            return result;
        },
        [&](const WhileStatement &whileStatement) -> std::string {
            std::string result = pad + "while (" + printExpression(*whileStatement.condition) + ")\n";
            result += printBlockStatement(*whileStatement.thenBranch, indent);
            return result;
        },
        [&](const ArchMap &archMap) -> std::string {
            std::string result = pad + "arch " + archMap.identifier + " {\n";
            for (std::size_t i = 0; i < archMap.registers.size(); ++i) {
                const auto& registers = archMap.registers[i];
                result += "    register " + registers.identifier + " = " + registers.physicalRegisterName;
                result += (i + 1 == archMap.registers.size()) ? "\n" : ",\n";
            }
            for (std::size_t i = 0; i < archMap.opcodes.size(); ++i) {
                 const auto& opcodes = archMap.opcodes[i];
                 result += "    opcode " + opcodes.identifier + " = " + opcodes.realOpcodeName;
                 result += (i + 1 == archMap.opcodes.size()) ? "\n" : ",\n";
            }
            result += "}\n";
            return result;
        },
        [&](const MachineDefinition &machineDefinition) -> std::string {
            return "machine " + machineDefinition.identifier + " "
            + printParameters(machineDefinition.parameters) + " "
            + printBlockStatement(*machineDefinition.block, indent);
        },
        [&](const FunctionDefinition &functionDefinition) -> std::string {
            return functionDefinition.type.name + " "
            + functionDefinition.identifier + " "
            + printParameters(functionDefinition.parameters) + " "
            + printBlockStatement(*functionDefinition.block, indent);
        },
        [&](const ExpressionStatement &expressionStatement) -> std::string {
            return "expressionStatement";
        },
        [&](const VariableDeclaration &variableDeclaration) -> std::string {
            std::string result = variableDeclaration.type.name;
            if (variableDeclaration.type.isPointer) result += '*';
            result += variableDeclaration.identifier + " = " ;
            return result;
        }

    }, statement);
}