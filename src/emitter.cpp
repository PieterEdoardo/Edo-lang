#include "emitter.hpp"

void Emitter::emitByte(uint8_t value) {
    bytes.push_back(value);
}

void Emitter::emitUnsigned32(uint32_t value) {
    for (int shift = 0; shift < 32; shift += 8) {
        emitByte(static_cast<uint8_t>((value >> shift) & 0xFF));
    }
}

void Emitter::emitMoveImmediate(const PhysicalRegister destination, const std::int32_t value) {
    const auto registerNumber = static_cast<std::uint8_t>(destination);
    emitByte(0x48 | (registerNumber >> 3));
    emitByte(0xC7);
    emitByte(0xC0  | (registerNumber & 0b111));
    emitUnsigned32(static_cast<std::uint32_t>(value));
}

void Emitter::emitSyscall() {
    emitByte(0x0F);
    emitByte(0x05);
}