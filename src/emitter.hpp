#pragma once
#include <cstdint>
#include <vector>

#include "ast.hpp"

enum class PhysicalRegister : std::uint8_t {
    Rax = 0, Rcx = 1, Rdx = 2, Rbx = 3, Rsp = 4, Rbp = 5, Rsi = 6, Rdi = 7,
    R8 = 8, R9 = 9, R10 = 10, R11 = 11, R12 = 12, R13 = 13, R14 = 14, R15 = 15
};

struct DataFixup {
    std::size_t displacementOffset;
    std::size_t dataOffset;
};

class Emitter {
public:
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> data;
    std::vector<DataFixup> fixups;

    void emitByte(uint8_t value);
    void emitUnsigned32(std::uint32_t value);
    void emitMoveImmediate(PhysicalRegister destination, std::int32_t value);
    void emitMoveRegister(PhysicalRegister destination, PhysicalRegister source);
    void emitStatement(const Statement& statement);
    void emitSyscall();
    void emitLoadAddress(PhysicalRegister destination, std::size_t dataOffset);

    std::size_t addString(const std::string& text);
    std::vector<std::uint8_t> finish() const;
};