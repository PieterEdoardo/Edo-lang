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
    Colon,
    Equals,
    Plus,
    Minus,
    Star,
    BSlash,
    FSlash,
    Modulo,
    DQuote,
    SQuote,

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