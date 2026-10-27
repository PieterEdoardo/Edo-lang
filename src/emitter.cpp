#include "emitter.hpp"

#include <optional>
#include <unordered_map>

#include "ast.hpp"
#include "token.hpp"

namespace {
    std::optional<PhysicalRegister> registerFromName(const std::string& name) {
        static const std::unordered_map<std::string, PhysicalRegister> registers = {
            {"rax", PhysicalRegister::Rax},
            {"rcx", PhysicalRegister::Rcx},
            {"rdx", PhysicalRegister::Rdx},
            {"rbx", PhysicalRegister::Rbx},
            {"rsp", PhysicalRegister::Rsp},
            {"rbp", PhysicalRegister::Rbp},
            {"rsi", PhysicalRegister::Rsi},
            {"rdi", PhysicalRegister::Rdi},
            {"r8",  PhysicalRegister::R8},
            {"r9",  PhysicalRegister::R9},
            {"r10", PhysicalRegister::R10},
            {"r11", PhysicalRegister::R11},
            {"r12", PhysicalRegister::R12},
            {"r13", PhysicalRegister::R13},
            {"r14", PhysicalRegister::R14},
            {"r15", PhysicalRegister::R15},
        };
        const auto found = registers.find(name);
        if (found == registers.end()) return std::nullopt;
        return found->second;
    }

    void emitAssignment(Emitter& emitter, const AssignmentStatement& assignment) {
        const auto* target = std::get_if<IdentifierExpression>(assignment.target.get());
        if (!target) {
            throw std::runtime_error("assignment target must be a plain identifier for now");
        }

        const std::optional<PhysicalRegister> destination = registerFromName(target->name);
        if (!destination) {
            throw std::runtime_error("Unknown register: " + target->name);
        }

        // imm assignment
        if (const auto* number = std::get_if<NumberExpression>(assignment.value.get())) {
            emitter.emitMoveImmediate(*destination, number->value);
            return;
        }

        // reg assignment
        if (const auto* sourceIdentifier = std::get_if<IdentifierExpression>(assignment.value.get())) {
            const std::optional<PhysicalRegister> source = registerFromName(sourceIdentifier->name);
            if (!source) {
                throw std::runtime_error("Unknown register: " + sourceIdentifier->name);
            }
            emitter.emitMoveRegister(*destination, *source);
            return;
        }
        throw std::runtime_error("Unsupported assignment");
    }
}

void Emitter::emitStatement(const Statement& statement) {
    if (std::get_if<SyscallStatement>(&statement)) {
        emitSyscall();
        return;
    }

    if (const auto* assignment = std::get_if<AssignmentStatement>(&statement)) {
        emitAssignment(*this, *assignment);
        return;
    }

    throw std::runtime_error("code generation not implemented for this statement yet");
}

void Emitter::emitByte(const uint8_t value) {
    bytes.push_back(value);
}

void Emitter::emitUnsigned32(const uint32_t value) {
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

void Emitter::emitMoveRegister(const PhysicalRegister destination, const PhysicalRegister source) {
    const auto destinationNumber = static_cast<std::uint8_t>(destination);
    const auto sourceNumber = static_cast<std::uint8_t>(source);
    emitByte(0x48 | ((sourceNumber >> 3) << 2) | destinationNumber >> 3);
    emitByte(0x89);
    emitByte(0xC0  | (sourceNumber & 0b111) << 3 | (destinationNumber & 0b111));

}

void Emitter::emitSyscall() {
    emitByte(0x0F);
    emitByte(0x05);
}

