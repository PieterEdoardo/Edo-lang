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

std::string printExpression(Expression& expression) {
    return std::visit(Overloaded{
        [](const NumberExpression& number) -> std::string {
            return std::to_string(number.value);
        },
        [](const IdentifierExpression& identifier) -> std::string {
            return identifier.name;
        },
        [](const BinaryExpression& binary) -> std::string {
            return "(" + printExpression(*binary.left) + " " +
                binary.operatorSymbol + " " +
                printExpression(*binary.right) + ")";
        },
        [](const UnaryExpression& unary) -> std::string {
            return "(" + unary.operatorSymbol + " " + printExpression(*unary.operand) + ")";
        }
    }, expression);
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


std::string printStatement(Statement& statement, const int indent) {
    const std::string pad(indent * 2, ' ');

    return std::visit(Overloaded{
        [&](const AssignmentStatement& assignment) -> std::string {
            return pad + assignment.target + " = " +
                   printExpression(*assignment.value) + ";\n";
        },
        [&](const BlockStatement& block) -> std::string {
            return printBlockStatement(block, indent);
        },
        [&](const IfStatement& ifStatement) -> std::string {
            std::string result = pad + "if (" +
                                  printExpression(*ifStatement.condition) + ")\n";
            result += printBlockStatement(*ifStatement.thenBranch, indent);
            if (ifStatement.elseBranch) {
                result += pad + "else\n";
                result += printBlockStatement(*ifStatement.elseBranch, indent);
            }
            return result;
        },
        [&](const WhileStatement& whileStatement) -> std::string {
            std::string result = pad + "while (" +
                                  printExpression(*whileStatement.condition) + ")\n";
            result += printBlockStatement(*whileStatement.thenBranch, indent);
            return result;
        }
    }, statement);
}