#include "parser.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <iostream>

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

bool Parser::checkAhead(const TokenType type, const std::size_t offset) const {
    if (position + offset >= tokens.size()) {
        return false;
    }
    return tokens[position + offset].type == type;
}

bool Parser::checkAny(const std::initializer_list<TokenType> types) const {
    for (const TokenType type : types) {
        if (check(type)) {
            return true;
        }
    }

    return false;
}

bool Parser::isTypeToken(const TokenType type) {
    return type == TokenType::Int ||
           type == TokenType::Char ||
           type == TokenType::Void;
}

std::size_t Parser::byteSizeForType(const TokenType type) {
    switch (type) {
        case TokenType::Char:   return 1;
        case TokenType::Int:    return 8;
        case TokenType::Void:   return 0;
        default:
            throw std::runtime_error("byteSizeForType called on a non-type token");
    }
}

const Token & Parser::expect(const TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }

    const Token& token = peek();
    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": expected '" + message + ", got \"" + token.lexeme + "\""
    );
}

StatementPointer Parser::parseStatement() {
    // std::string result = isTypeToken(peek().type) ? "yes " : "no ";
    // std::cout << result;

    // std::cout << "token: \"" << peek().lexeme << "\" type=" << static_cast<int>(peek().type) << "\n";
    // Typed declarations
    if (isTypeToken(peek().type)) {
        // std::cout << "test";
        std::size_t identifierOffset = 1;
        if (checkAhead(TokenType::Star, 1)) identifierOffset = 2;

        if (checkAhead(TokenType::Identifier, identifierOffset) && checkAhead(TokenType::LParen, identifierOffset + 1)) {
            return parseFunctionDefinition();
        }

        if (checkAhead(TokenType::Identifier, identifierOffset)) {
            return parseVariableDeclaration();
        }
    }

    // Double checks
    if (check(TokenType::Identifier) && checkAhead(TokenType::LParen, 1)) {
        return parseCallStatement();
    }

    // Single checks
    if (check(TokenType::Machine)) {
        return parseMachineDefinition();
    }

    if (check(TokenType::If)) {
        return parseIfStatement();
    }

    if (check(TokenType::While)) {
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
    if (check(TokenType::Arch)) {
        return parseArchMap();
    }

    return parseAssignmentStatement();
}

TypeDefinition Parser::parseTypeDefinition(const bool useRegisters) {
    bool isPointer = false;
    bool isRegister = false;
    Token typeTarget;

    if (isTypeToken(peek().type)) {
        typeTarget = advance();
        if (check(TokenType::Star)) {
            advance();
            isPointer = true;
        }
    } else if (check(TokenType::Identifier) && useRegisters) {
        // Assume register typing is used
        typeTarget = expect(TokenType::Identifier, "register name");
        isRegister = true;
    } else {
        const Token& badToken = peek();
        throw std::runtime_error(
            "Parse error at line " + std::to_string(badToken.line) +
            ", column " + std::to_string(badToken.column) +
            ": expected a type ('int', 'char', or 'void'), or user defined register, got \"" + badToken.lexeme + "\""
        );
    }

    return TypeDefinition{
        .name = typeTarget.lexeme,
        .isPointer = isPointer,
        .isRegister = isRegister,
        .byteSize = isRegister ? 0 : byteSizeForType(typeTarget.type)
    };
}

ParameterDefinition Parser::parseParameters(const bool useRegisters) {
    expect(TokenType::LParen, "Expected '(' after machine statement identifier");

    std::vector<TypeDefinition> typeList;
    std::vector<std::string> identifierList;

    while (!check(TokenType::RParen)) {
        TypeDefinition typeTarget = parseTypeDefinition(useRegisters);
        const Token& typeIdentifierTarget = expect(TokenType::Identifier, "identifier after type definition of parameter declaration");

        typeList.push_back(std::move(typeTarget));
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

std::vector<ExpressionPointer> Parser::parseArguments() {

    expect(TokenType::LParen, "parentheses at beginning of arguments");

    std::vector<ExpressionPointer> arguments;

    while (!check(TokenType::RParen)) {
        arguments.push_back(parseExpression());

        if (check(TokenType::Comma)) advance();
    }

    expect(TokenType::RParen, "')' at end of arguments");

    return arguments;
}

StatementPointer Parser::parseCallStatement() {
    const Token& token = expect(TokenType::Identifier, "identifier at start of call");

    std::vector<ExpressionPointer> arguments = parseArguments();

    expect(TokenType::Semicolon, "';' after call statement");

    return std::make_unique<Statement>(ExpressionStatement{
        .expression = std::make_unique<Expression>(CallExpression{
            .identifier = token.lexeme,
            .arguments = std::move(arguments)
        })
    });
}

StatementPointer Parser::parseFunctionDefinition() {
    TypeDefinition typeTarget = parseTypeDefinition(false);

    const Token token = expect(TokenType::Identifier, "identifier after function definition");

    ParameterDefinition parameters = parseParameters(false);

    const StatementPointer blockStatement = parseBlockStatement();
    auto block = std::make_unique<BlockStatement>(
      std::move(std::get<BlockStatement>(*blockStatement))
    );

    return std::make_unique<Statement>(FunctionDefinition{
        .type = std::move(typeTarget),
        .identifier = token.lexeme,
        .parameters = std::move(parameters),
        .block = std::move(block)
    });
}

// Calls
StatementPointer Parser::parseMachineDefinition() {
    expect(TokenType::Machine, "'machine' at start of machine statement");

    const Token& token = expect(TokenType::Identifier, "identifier after machine statement");

    ParameterDefinition parameters = parseParameters(true);

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

StatementPointer Parser::parseVariableDeclaration() {
    TypeDefinition typeTarget = parseTypeDefinition(false);

    if (check(TokenType::Star)) {
        advance();
        typeTarget.isPointer = true;
    }

    const Token& identifierToken = expect(TokenType::Identifier, "identifier after type in declaration");

    ExpressionPointer initializer = nullptr;
    if (check(TokenType::Equals)) {
        advance();
        initializer = parseExpression();
    }

    expect(TokenType::Semicolon, "';' after declaration");

    return std::make_unique<Statement>(VariableDeclaration{
        .type = std::move(typeTarget),
        .identifier = identifierToken.lexeme,
        .initializer = std::move(initializer)
    });
}

// Architecture
StatementPointer Parser::parseArchMap() {
    expect(TokenType::Arch, "'arch' at start of arch statement");

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
    Token targetToken;
    bool dereference = false;
    if (check(TokenType::Star) && checkAhead(TokenType::Identifier, 1)) {
        expect(TokenType::Star, "'*' operator at start of assignment statement");
        targetToken = expect(TokenType::Identifier, "identifier at start of assignment statement");
        dereference = true;
    } else if (check(TokenType::Identifier)) {
        targetToken = expect(TokenType::Identifier, "identifier at start of assignment statement");
    } else {
        const Token& badToken = peek();
        throw std::runtime_error(
            "Parse error at line " + std::to_string(badToken.line) +
            ", column " + std::to_string(badToken.column) +
            ": expected identifier or '*identifier' at start of assignment statement, got \"" + badToken.lexeme + "\""
        );
    }

    const std::string targetName = targetToken.lexeme;

    expect(TokenType::Equals, "Expected '=' after identifier in assignment statement.");

    ExpressionPointer value = parseExpression();

    expect(TokenType::Semicolon, "Expected ';' after assignment statement.");

    return std::make_unique<Statement>(AssignmentStatement{
        .target = targetName,
        .dereference = dereference,
        .value = std::move(value)
    });
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
    expect(TokenType::If, "'if' at start of if statement");
    expect(TokenType::LParen, "'(' after 'if'");

    ExpressionPointer condition = parseExpression();

    expect(TokenType::RParen, "')' after if condition");

    const StatementPointer thenStatement = parseBlockStatement();
    auto thenBranch = std::make_unique<BlockStatement>(
        std::move(std::get<BlockStatement>(*thenStatement))
    );

    StatementPointer elseBranch = nullptr;
    if (check(TokenType::Else)) {
        advance();
        if (check(TokenType::If)) {
            elseBranch = parseIfStatement();
        } else {
            elseBranch = parseBlockStatement();
        }
    }

    return std::make_unique<Statement>(IfStatement{
        .condition = std::move(condition),
        .thenBranch = std::move(thenBranch),
        .elseBranch = std::move(elseBranch)}
    );
}

StatementPointer Parser::parseWhileStatement() {
    expect(TokenType::While, "'while' at start of while statement");
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
    if (check(TokenType::String)) {
        advance();
        return std::make_unique<Expression>(StringExpression{token.lexeme});
    }

    if (check(TokenType::Number)) {
        advance();
        const int value = std::stoi(token.lexeme);
        return std::make_unique<Expression>(NumberExpression{value});
    }

    if (check(TokenType::Identifier)) {
        if (checkAhead(TokenType::LParen, 1)) {
            const Token& identifierToken = advance();
            std::vector<ExpressionPointer> arguments = parseArguments();

            return std::make_unique<Expression>(CallExpression{
                .identifier = identifierToken.lexeme,
                .arguments = std::move(arguments)
            });
        }

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

    if (isTypeToken(peek().type)) {
        TypeDefinition type = parseTypeDefinition(false);
        return std::make_unique<Expression>(TypeExpression{
            .type = std::move(type)
        });
    }

    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": expected expression, got \"" + token.lexeme + "\""
    );
}

// Operators

ExpressionPointer Parser::parseUnary() {
    bool isCast = false;
    if (constexpr std::size_t offset = 1; position + offset < tokens.size() && isTypeToken(tokens[position + offset].type)) {
        isCast = true;
    }

    if (check(TokenType::LParen) && isCast) {
        bool isPointer = false;
        advance();
        TypeDefinition castType = parseTypeDefinition(false);
        if (check(TokenType::Star)) {
            isPointer = true;
            advance();
        }
        expect(TokenType::RParen,"')' after cast type");
        ExpressionPointer operand = parseUnary();

        return std::make_unique<Expression>(CastExpression{
            .type = std::move(castType),
            .operand = std::move(operand)
        });
    }

    if (checkAny({TokenType::Not, TokenType::Ampersand, TokenType::Star})) {
        const Token& operatorToken = advance();
        ExpressionPointer operand = parseUnary();

        return std::make_unique<Expression>(UnaryExpression{
            .operatorSymbol = operatorToken.lexeme,
            .operand = std::move(operand)
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

