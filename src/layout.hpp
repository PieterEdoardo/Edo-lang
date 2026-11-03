// src/layout.hpp
#pragma once
#include <cstdint>

namespace Layout {
    constexpr std::uint64_t loadAddress = 0x400000;
    constexpr std::uint64_t elfHeaderSize = 64;
    constexpr std::uint64_t programHeaderSize = 56;
    constexpr std::uint64_t programHeaderCount = 2;   // PT_LOAD + PT_GNU_STACK
    constexpr std::uint64_t codeOffset = elfHeaderSize + programHeaderSize * programHeaderCount; // 176
}
