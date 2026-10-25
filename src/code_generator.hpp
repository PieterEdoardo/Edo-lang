// src/code_generator.hpp
#pragma once
#include "ast.hpp"
#include "emitter.hpp"

void generateStatement(Emitter& emitter, const Statement& statement);