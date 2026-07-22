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
    KwElse,
    KwFor,
    KwReturn,
    KwBreak,
    KwContinue,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Semicolon,
    Equals,
    Plus,
    Minus,
    Star,
    Slash,

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