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
    char character = source[position++];

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
        char character = peek();

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

        int startLine = line;
        int startColumn = column;
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
            case '"': tokens.push_back(makeToken(TokenType::DQuote, "\"", startLine, startColumn)); break;
            case '\'': tokens.push_back(makeToken(TokenType::SQuote, "'", startLine, startColumn)); break;
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
                Token invalid{TokenType::Invalid, std::string(1, character), startLine, startColumn};
                tokens.push_back(invalid);
            }
        }
    }

    tokens.push_back(Token{TokenType::EndOfFile, "", line, column});
    return tokens;
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, int tokenLine, int tokenColumn) {
    return Token{type, lexeme, tokenLine, tokenColumn};
}

Token Lexer::lexNumber() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;

    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    std::string lexeme = source.substr(start, position - start);

    return Token{TokenType::Number, lexeme, startLine, startColumn};
}

Token Lexer::lexIdentifierOrKeyword() {
    int startLine = line;
    int startColumn = column;
    size_t start = position;

    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        advance();
    }

    std::string lexeme = source.substr(start, position - start);

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"if", TokenType::KwIf},
        {"else", TokenType::KwElse},
        {"for", TokenType::KwFor},
        {"while", TokenType::KwWhile},
        {"syscall", TokenType::KwSyscall},
        {"return", TokenType::KwReturn},
        {"break", TokenType::KwBreak},
        {"continue", TokenType::KwContinue},
    };

    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token{it->second, lexeme, startLine, startColumn};
    }

    return Token{TokenType::Identifier, lexeme, startLine, startColumn};
}