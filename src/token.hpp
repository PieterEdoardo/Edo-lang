#pragma once
#include <string>

enum class TokenType {
    // Literals
    Number,
    Identifier,
    String,

    // Types
    Int,
    Char,
    Void,

    // Architecture
    Register,
    Opcode,

    // Keywords
    Machine,
    Arch,
    If,
    Else,
    While,
    For,
    Break,
    Continue,
    Return,

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
    Pipe,
    Or,
    And,

    // Unary operators
    Not,
    Ampersand,

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