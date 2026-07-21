#pragma once
#include <string>

enum class TokenType {
    // Literals
    Number,
    Identifier,

    // Keywords
    KwSyscall,
    KwIf,
    KwWhile,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    Semicolon,
    Equals,
    Plus,
    Minus,

    // End of stream
    EndOfFile,

    // Error token
    Invalid,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};