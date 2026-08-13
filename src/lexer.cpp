#include "lexer.hpp"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(std::string source) : source(std::move(source)) {}

bool Lexer::isAtEnd() const {
    return position >= source.size();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::advance() {
    const char character = source[position++];

    if (character == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    return character;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        const char character = peek();

        if (std::isspace(static_cast<unsigned char>(character))) {
            advance();
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(character))) {
            tokens.push_back(lexNumber());
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(character)) || character == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
            continue;
        }

        if (character == '"') {
            tokens.push_back(lexString());
            continue;
        }

        const int startLine = line;
        const int startColumn = column;
        advance();

        switch (character) {
            case '(': tokens.push_back(makeToken(TokenType::LParen, "(", startLine, startColumn)); break;
            case ')': tokens.push_back(makeToken(TokenType::RParen, ")", startLine, startColumn)); break;
            case '{': tokens.push_back(makeToken(TokenType::LBrace, "{", startLine, startColumn)); break;
            case '}': tokens.push_back(makeToken(TokenType::RBrace, "}", startLine, startColumn)); break;
            case ';': tokens.push_back(makeToken(TokenType::Semicolon, ";", startLine, startColumn)); break;
            case '+': tokens.push_back(makeToken(TokenType::Plus, "+", startLine, startColumn)); break;
            case '-': tokens.push_back(makeToken(TokenType::Minus, "-", startLine, startColumn)); break;
            case '*': tokens.push_back(makeToken(TokenType::Star, "*", startLine, startColumn)); break;
            case '%': tokens.push_back(makeToken(TokenType::Modulo, "%", startLine, startColumn)); break;
            case '\\': tokens.push_back(makeToken(TokenType::BSlash, "\\", startLine, startColumn)); break;
            case '/': tokens.push_back(makeToken(TokenType::FSlash, "/", startLine, startColumn)); break;
            case '\'': tokens.push_back(makeToken(TokenType::SQuote, "'", startLine, startColumn)); break;
            case ',': tokens.push_back(makeToken(TokenType::Comma, ",", startLine, startColumn)); break;
            case '|': tokens.push_back(makeToken(TokenType::Pipe, "|", startLine, startColumn)); break;
            case '&': tokens.push_back(makeToken(TokenType::Ampersand, "&", startLine, startColumn)); break;
            case '=': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::EqualEqual, "==", startLine, startColumn));
                } else {
                    tokens.push_back(makeToken(TokenType::Equals, "=", startLine, startColumn));
                }
                break;
            }
            case '!': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::NotEqual, "!=", startLine, startColumn));
                } else {
                    tokens.push_back(makeToken(TokenType::Not, "!", startLine, startColumn));
                }
                break;
            }
            case '<': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LesserEqual, "<=", startLine, startColumn));
                } else {
                    tokens.push_back(makeToken(TokenType::Lesser, "<", startLine, startColumn));
                }
                break;
            }
            case '>': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::GreaterEqual, ">=", startLine, startColumn));
                } else {
                    tokens.push_back(makeToken(TokenType::Greater, ">", startLine, startColumn));
                }
                break;
            }
            default: {
                Token invalid{
                    .type = TokenType::Invalid,
                    .lexeme = std::string(1, character),
                    .line = startLine,
                    .column = startColumn
                };
                tokens.push_back(invalid);
            }
        }
    }

    tokens.push_back(Token{
        .type = TokenType::EndOfFile,
        .lexeme = "",
        .line = line,
        .column = column
    });
    return tokens;
}

Token Lexer::makeToken(const TokenType type, const std::string& lexeme, const int tokenLine, const int tokenColumn) {
    return Token{
        .type = type,
        .lexeme = lexeme,
        .line = tokenLine,
        .column = tokenColumn
    };
}

Token Lexer::lexNumber() {
    const int startLine = line;
    const int startColumn = column;
    const size_t start = position;

    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    const std::string lexeme = source.substr(start, position - start);

    return Token{
        .type = TokenType::Number,
        .lexeme = lexeme,
        .line = startLine,
        .column = startColumn
    };
}

Token Lexer::lexString() {
    const int startLine = line;
    const int startColumn = column;

    advance();
    const size_t start = position;

    while (!isAtEnd() && peek() != '"') advance();

    const std::string value = source.substr(start, position - start);

    if (!isAtEnd()) advance();

    return Token{
        .type = TokenType::String,
        .lexeme = value,
        .line = startLine,
        .column = startColumn

    };
}

Token Lexer::lexIdentifierOrKeyword() {
    const int startLine = line;
    const int startColumn = column;
    const size_t start = position;

    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        advance();
    }

    const std::string lexeme = source.substr(start, position - start);

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"machine",     TokenType::Machine},
        {"if",          TokenType::If},
        {"else",        TokenType::Else},
        {"for",         TokenType::For},
        {"while",       TokenType::While},
        {"return",      TokenType::Return},
        {"break",       TokenType::Break},
        {"continue",    TokenType::Continue},
        {"arch",        TokenType::Arch},
        {"register",    TokenType::Register},
        {"opcode",      TokenType::Opcode},
        {"int",         TokenType::Int},
        {"char",        TokenType::Char},
        {"void",        TokenType::Void},
    };

    if (const auto it = keywords.find(lexeme); it != keywords.end()) {
        return Token{
            .type = it->second,
            .lexeme = lexeme,
            .line = startLine,
            .column = startColumn
        };
    }

    return Token{
        .type = TokenType::Identifier,
        .lexeme = lexeme,
        .line = startLine,
        .column = startColumn
    };
}

