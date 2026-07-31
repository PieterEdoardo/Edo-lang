#pragma once
#include "ast.hpp"
#include <string>

std::string printExpression(Expression& expression);
std::string printStatement(Statement& statement, int indent = 0);
std::string printBlockStatement(Statement& statement, int indent);
std::string printArchMap(ArchMap& archMap);