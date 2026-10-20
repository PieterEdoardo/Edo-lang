#pragma once
#include <cstdint>
#include <vector>

enum class PhysicalRegister : std::uint8_t {
    Rax = 0, Rcx = 1, Rdx = 2, Rbx = 3, Rsp = 4, Rbp = 5, Rsi = 6, Rdi = 7,
    R8 = 8, R9 = 9, R10 = 10, R11 = 11, R12 = 12, R13 = 13, R14 = 14, R15 = 15
};

class Emitter {
public:
    std::vector<uint8_t> bytes;

    void emitByte(uint8_t value);
    void emitUnsigned32(std::uint32_t value);
    void emitMoveImmediate(PhysicalRegister destination, std::int32_t value);
    void emitSyscall();
};