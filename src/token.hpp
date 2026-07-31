#pragma once
#include <string>

enum class TokenType {
    // Literals
    Number,
    Identifier,
    Type,

    // Architecture
    Register,
    Opcode,

    // Keywords
    KwMachine,
    KwArch,
    KwIf,
    KwElse,
    KwWhile,
    KwFor,
    KwBreak,
    KwContinue,
    KwReturn,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Semicolon,
    Colon,
    Comma,
    Dot,

    // Operators
    Equals,
    Plus,
    Minus,
    Star,
    BSlash,
    FSlash,
    Modulo,
    Lesser,
    Greater,

    // Unary operators
    Not,

    // Quotes
    DQuote,
    SQuote,

    // Combination types
    EqualEqual,
    NotEqual,
    LesserEqual,
    GreaterEqual,

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