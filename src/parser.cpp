#include "parser.hpp"
#include <memory>
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

ExpressionPointer Parser::parseExpression() {
    return parseComparison();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() const {
    return tokens[position];
}

const Token& Parser::advance() {
    const Token& token = tokens[position];

    if (!isAtEnd()) {
        position++;
    }

    return token;
}

bool Parser::check(const TokenType type) const {
    return peek().type == type;
}

bool Parser::isTypeToken(const TokenType type) {
    return type == TokenType::Int ||
           type == TokenType::Char ||
           type == TokenType::Void;
}

const Token & Parser::expect(TokenType type, const std::string& expected) {
    if (check(type)) {
        return advance();
    }

    const Token& token = peek();
    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": expected '" + expected + ", got \"" + token.lexeme + "\""
    );
}

StatementPointer Parser::parseStatement() {
    if (check(TokenType::KwMachine)) {
        return parseMachineDefinition();
    }

    if (check(TokenType::KwIf)) {
        return parseIfStatement();
    }

    if (check(TokenType::KwWhile)) {
        return parseWhileStatement();
    }

    // if (check(TokenType::KwFor)) {
    //     return parseForStatement();
    // }

    // if (check(TokenType::KwBreak)) {
    //     return parseBreakStatement();
    // }

    // if (check(TokenType::KwContinue)) {
    //     return parseContinueStatement();
    // }

    // if (check(TokenType::KwReturn)) {
    //     return parseReturnStatement();
    // }

    if (check(TokenType::LBrace)) {
        return parseBlockStatement();
    }

    // Architecture
    if (check(TokenType::KwArch)) {
        return parseArchMap();
    }

    return parseAssignmentStatement();
}

ParameterDefinition Parser::parseParameters() {
    expect(TokenType::LParen, "Expected '(' after machine statement identifier");

    std::vector<TypeDefinition> typeList;
    std::vector<std::string> identifierList;

    while (!check(TokenType::RParen)) {

        if (!isTypeToken(peek().type)) {
            const Token& badToken = peek();
            throw std::runtime_error(
                "Parse error at line " + std::to_string(badToken.line) +
                ", column " + std::to_string(badToken.column) +
                ": expected a type ('int', 'char', or 'void'), got \"" + badToken.lexeme + "\""
            );
        }

        const Token& typeTarget = advance();
        const Token& typeIdentifierTarget = expect(TokenType::Identifier, "identifier after type definition of parameter declaration");

        typeList.push_back(TypeDefinition{
            .name = typeTarget.lexeme,
            .byteSize = 8
        });
        identifierList.emplace_back(typeIdentifierTarget.lexeme);

        if (check(TokenType::Comma)) {
            advance();
        }
    }

    expect(TokenType::RParen, "')' after final parameter declaration");

    return ParameterDefinition{
        .types = std::move(typeList),
        .identifiers = std::move(identifierList)
    };
}

// Calls
StatementPointer Parser::parseMachineDefinition() {
    expect(TokenType::KwMachine, "'machine' at start of machine statement");

    const Token& token = expect(TokenType::Identifier, "identifier after machine statement");

    ParameterDefinition parameters = parseParameters();

    const StatementPointer blockStatement = parseBlockStatement();
    auto block = std::make_unique<BlockStatement>(
      std::move(std::get<BlockStatement>(*blockStatement))
    );

    return std::make_unique<Statement>(MachineDefinition{
        .identifier = token.lexeme,
        .parameters = std::move(parameters),
        .block = std::move(block)
    });
}

// Architecture
StatementPointer Parser::parseArchMap() {
    expect(TokenType::KwArch, "'arch' at start of arch statement");

    const Token& targetToken = expect(TokenType::Identifier, "identifier after arch statement");

    expect(TokenType::LBrace, "Expected '{' after arch statement.");

    std::vector<RegisterMap> registerMap;
    std::vector<OpcodeMap> opcodeMap;

    while (!check(TokenType::RBrace) && !isAtEnd()) {
        if (check(TokenType::Register)) {
            expect(TokenType::Register, "'register' at start of register in arch map");

            const Token& registerToken = expect(TokenType::Identifier, "identifier at start of register map");
            const std::string registerName = registerToken.lexeme;

            expect(TokenType::Equals, "Expected '=' after identifier in assignment statement.");

            const Token& actualRegisterToken = expect(TokenType::Identifier, "identifier at start of register map");
            const std::string actualRegisterName = actualRegisterToken.lexeme;

            registerMap.push_back(RegisterMap{
                .identifier = registerName,
                .physicalRegisterName = actualRegisterName,
            });

            if (check(TokenType::Comma)) {
                advance();
            }
        } else if (check(TokenType::Opcode)) {
            expect(TokenType::Opcode, "'opcode' at start of opcode in arch map");

            const Token& opcodeToken = expect(TokenType::Identifier, "identifier at start of opcode map");
            const std::string opcodeName = opcodeToken.lexeme;

            expect(TokenType::Equals, "Expected '=' after identifier in assignment statement.");

            const Token& actualOpcodeToken = expect(TokenType::Identifier, "identifier at start of opcode map");
            const std::string actualOpcodeName = actualOpcodeToken.lexeme;

            opcodeMap.push_back(OpcodeMap{
                .identifier = opcodeName,
                .realOpcodeName = actualOpcodeName,
            });

            if (check(TokenType::Comma)) {
                advance();
            }
        } else {
            const Token& badToken = peek();
            throw std::runtime_error(
                "Parse error at line " + std::to_string(badToken.line) +
                ", column " + std::to_string(badToken.column) +
                ": expected 'register' or 'opcode' in arch block, got \"" + badToken.lexeme + "\""
            );
        }
    }

    expect(TokenType::RBrace, "Expected '}' at end of block statement.");

    return std::make_unique<Statement>(ArchMap{
        .identifier = targetToken.lexeme,
        .registers = std::move(registerMap),
        .opcodes = std::move(opcodeMap),
    });
}

// Grouped logic
StatementPointer Parser::parseAssignmentStatement() {
    const Token& targetToken = expect(TokenType::Identifier, "identifier at start of assignment statement");
    const std::string targetName = targetToken.lexeme;

    expect(TokenType::Equals, "Expected '=' after identifier in assignment statement.");

    ExpressionPointer value = parseExpression();

    expect(TokenType::Semicolon, "Expected ';' after assignment statement.");

    return std::make_unique<Statement>(AssignmentStatement{targetName, std::move(value)});
}

StatementPointer Parser::parseBlockStatement() {
    expect(TokenType::LBrace, "Expected '{' after block statement.");

    std::vector<StatementPointer> statements;

    while (!check(TokenType::RBrace) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }

    expect(TokenType::RBrace, "Expected '}' at end of block statement.");

    return std::make_unique<Statement>(BlockStatement{std::move(statements)});
}

ExpressionPointer Parser::parseComparison() {
    ExpressionPointer left = parseAdditive();

    if (check(TokenType::EqualEqual) || check(TokenType::NotEqual) ||
        check(TokenType::Lesser) || check(TokenType::Greater) ||
        check(TokenType::LesserEqual) || check(TokenType::GreaterEqual)) {

        const Token& operatorToken = advance();
        ExpressionPointer right = parseAdditive();

        return std::make_unique<Expression>(BinaryExpression{
            .left = std::move(left),
            .operatorSymbol = operatorToken.lexeme,
            .right = std::move(right)
        });
    }

    return left;
}

// Statements
StatementPointer Parser::parseIfStatement() {
    expect(TokenType::KwIf, "'if' at start of if statement");
    expect(TokenType::LParen, "'(' after 'if'");

    ExpressionPointer condition = parseExpression();

    expect(TokenType::RParen, "')' after if condition");

    const StatementPointer thenStatement = parseBlockStatement();
    auto thenBranch = std::make_unique<BlockStatement>(
        std::move(std::get<BlockStatement>(*thenStatement))
    );

    std::unique_ptr<BlockStatement> elseBranch = nullptr;
    if (check(TokenType::KwElse)) {
        advance();
        const StatementPointer elseStatement = parseBlockStatement();
        elseBranch = std::make_unique<BlockStatement>(
            std::move(std::get<BlockStatement>(*elseStatement))
        );
    }

    return std::make_unique<Statement>(IfStatement{
        .condition = std::move(condition),
        .thenBranch = std::move(thenBranch),
        .elseBranch = std::move(elseBranch)}
    );
}

StatementPointer Parser::parseWhileStatement() {
    expect(TokenType::KwWhile, "'while' at start of while statement");
    expect(TokenType::LParen, "'(' after 'while'");

    ExpressionPointer condition = parseExpression();

    expect(TokenType::RParen, "')' after while condition");

    const StatementPointer thenStatement = parseBlockStatement();
    auto thenBranch = std::make_unique<BlockStatement>(
        std::move(std::get<BlockStatement>(*thenStatement))
    );

    return std::make_unique<Statement>(
        WhileStatement{
        .condition = std::move(condition),
        .thenBranch = std::move(thenBranch)
    });
}

ExpressionPointer Parser::parsePrimary() {
    const Token& token = peek();

    // Character groups
    if (check(TokenType::Number)) {
        advance();
        int value = std::stoi(token.lexeme);
        return std::make_unique<Expression>(NumberExpression{value});
    }

    if (check(TokenType::Identifier)) {
        advance();
        return std::make_unique<Expression>(IdentifierExpression{token.lexeme});
    }

    // Character specific
    if (check(TokenType::LParen)) {
        advance();
        ExpressionPointer inner = parseExpression();

        if (!check(TokenType::RParen)) {
            const Token& badToken = peek();
            throw std::runtime_error(
                "Parse error at line " + std::to_string(badToken.line) +
                ", column " + std::to_string(badToken.column) +
                ": expected ')' after expression"
            );
        }
        advance();

        return inner;
    }

    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": expected expression, got \"" + token.lexeme + "\""
    );
}

// Operators

ExpressionPointer Parser::parseUnary() {
    if (check(TokenType::Not)) {
        const Token& operatorToken = advance();
        ExpressionPointer operand = parseUnary();

        return std::make_unique<Expression>(UnaryExpression{
            .operatorSymbol = operatorToken.lexeme, .operand = std::move(operand)
        });
    }

    return parsePrimary();
}

ExpressionPointer Parser::parseMultiplicative() {
    ExpressionPointer left = parseUnary();

    while (check(TokenType::Star) || check(TokenType::FSlash) || check(TokenType::Modulo)) {
        const Token& operatorToken = advance();
        ExpressionPointer right = parseUnary();

        left = std::make_unique<Expression>(BinaryExpression{
            .left = std::move(left),
            .operatorSymbol = operatorToken.lexeme,
            .right = std::move(right)
        });
    }

    return left;
}

ExpressionPointer Parser::parseAdditive() {
    ExpressionPointer left = parseMultiplicative();

    while (check(TokenType::Plus) || check(TokenType::Minus)) {
        const Token& operatorToken = advance();
        ExpressionPointer right = parseMultiplicative();

        left = std::make_unique<Expression>(BinaryExpression{
            .left = std::move(left),
            .operatorSymbol = operatorToken.lexeme,
            .right = std::move(right)
        });
    }

    return left;
}

