#include "emitter.hpp"
#include "code_generator.hpp"

#include <optional>
#include <unordered_map>


namespace {
    std::optional<PhysicalRegister> registerFromName(const std::string& name) {
        static const std::unordered_map<std::string, PhysicalRegister> registers = {
            {"rax", PhysicalRegister::Rax},
            {"rdx", PhysicalRegister::Rdx},
            {"rbx", PhysicalRegister::Rbx},
            {"rsp", PhysicalRegister::Rsp},
            {"rbp", PhysicalRegister::Rbp},
            {"rsi", PhysicalRegister::Rsi},
            {"rdi", PhysicalRegister::Rdi},
            {"r8", PhysicalRegister::R8},
            {"r9", PhysicalRegister::R9},
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

    void generateAssignment(Emitter& emitter, const AssignmentStatement& assignment) {
        // 1. The target must be a plain identifier. get_if returns a pointer to the
        //    alternative if the variant holds it, or nullptr if not:
        const auto* targetIdentifier = std::get_if<IdentifierExpression>(assignment.target.get());
        if (!targetIdentifier) {
            throw std::runtime_error("assignment target is not a plain identifier yet");
        }

        switch (targetIdentifier->name) {
            case
        }

        // 2. TODO: look up targetIdentifier->name with registerFromName().
        //    If it's not a register, throw an error (variables come later).

        // 3. TODO: look at assignment.value the same way with get_if:
        //      - NumberExpression      → emitter.emitMoveImmediate(destination, number->value)
        //      - IdentifierExpression  → if it names a register: emitter.emitMoveRegister(...)
        //      - anything else         → throw "not implemented yet"
    }
}
