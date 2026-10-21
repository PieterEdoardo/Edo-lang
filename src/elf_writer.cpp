#include "elf_writer.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {
    constexpr std::uint64_t loadAddress = 0x400000;
    constexpr std::uint64_t elfHeaderSize = 64;
    constexpr std::uint64_t programHeaderSize = 56;
    constexpr std::uint64_t codeOffset = elfHeaderSize + programHeaderSize;

    void appendLittleEndian(std::vector<std::uint8_t>& bytes, const std::uint64_t value, const int byteCount) {
        for (int index = 0; index < byteCount; ++index) {
            bytes.push_back(static_cast<std::uint8_t>(value >> (index * 8)));
        }
    }
}

void writeElfExecutable(const std::string& outputPath, const std::vector<std::uint8_t>& code) {
    std::vector<std::uint8_t> file;

    // ---- ELF header (64 bytes) ----
    file.insert(file.end(), {0x7F, 'E', 'L', 'F'});            // magic
    file.push_back(2);                                                   // 64-bit
    file.push_back(1);                                                   // little-endian
    file.push_back(1);                                                   // ELF version
    file.push_back(0);                                                   // OS ABI (System V)
    file.insert(file.end(), 8, 0);                           // padding up to 16 byte

    appendLittleEndian(file, 2, 2);                       // type: executable
    appendLittleEndian(file, 0x3E, 2);                    // machine: x86-64
    appendLittleEndian(file, 1, 4);                       // version
    appendLittleEndian(file, loadAddress + codeOffset, 8);// entry point
    appendLittleEndian(file, elfHeaderSize, 8);           // program header offset
    appendLittleEndian(file, 0, 8);                       // section header offset (none)
    appendLittleEndian(file, 0, 4);                       // flags
    appendLittleEndian(file, elfHeaderSize, 2);           // this header's size
    appendLittleEndian(file, programHeaderSize, 2);       // size of one program header
    appendLittleEndian(file, 1, 2);                       // number of program headers
    appendLittleEndian(file, 64, 2);                      // section header size (unused)
    appendLittleEndian(file, 0, 2);                       // number of section headers
    appendLittleEndian(file, 0, 2);                       // section name table index

    // ---- Program header (56 bytes) ----
    const std::uint64_t totalSize = codeOffset + code.size();
    appendLittleEndian(file, 1, 4);                       // type: PT_LOAD
    appendLittleEndian(file, 5, 4);                       // flags: read + execute
    appendLittleEndian(file, 0, 8);                       // offset in file (from the very start)
    appendLittleEndian(file, loadAddress, 8);             // virtual address
    appendLittleEndian(file, loadAddress, 8);             // physical address (ignored)
    appendLittleEndian(file, totalSize, 8);                    // bytes in file
    appendLittleEndian(file, totalSize, 8);                    // bytes in memory
    appendLittleEndian(file, 0x1000, 8);                  // alignment (page size)

    // ---- Code ----
    file.insert(file.end(), code.begin(), code.end());

    std::ofstream output(outputPath, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Could not open output file: " + outputPath);
    }
    output.write(reinterpret_cast<const char*>(file.data()), static_cast<std::streamsize>(file.size()));
    output.close();

    std::filesystem::permissions(
        outputPath,
        std::filesystem::perms::owner_exec |
        std::filesystem::perms::group_exec |
        std::filesystem::perms::others_exec,
        std::filesystem::perm_options::add
    );
}