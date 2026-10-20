#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ast_printer.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "emitter.hpp"


static void printHex(const std::vector<std::uint8_t>& bytes) {
    for (const std::uint8_t byte : bytes) {
        std::printf("%02X ", byte);
    }
    std::printf("\n");
}

static void expectBytes(const char* label,
                        const std::vector<std::uint8_t>& actual,
                        const std::vector<std::uint8_t>& expected) {
    std::cout << label << ": " << (actual == expected ? "PASS" : "FAIL") << "\n";
    if (actual != expected) {
        std::printf("  expected: "); printHex(expected);
        std::printf("  actual:   "); printHex(actual);
    }
}

int main() {
    {
        Emitter emitter;
        emitter.emitMoveImmediate(PhysicalRegister::Rdi, 42);
        emitter.emitMoveImmediate(PhysicalRegister::Rax, 60);
        emitter.emitSyscall();
        expectBytes("exit(42)", emitter.bytes,
            {0x48,0xC7,0xC7,0x2A,0x00,0x00,0x00,
             0x48,0xC7,0xC0,0x3C,0x00,0x00,0x00,
             0x0F,0x05});
    }
    {
        Emitter emitter;
        emitter.emitMoveImmediate(PhysicalRegister::R8, 1);
        expectBytes("mov r8, 1", emitter.bytes,
            {0x49,0xC7,0xC0,0x01,0x00,0x00,0x00});
    }
}
// static std::string readSourceFile(const std::string& path) {
//     std::ifstream file(path);
//     if (!file) {
//         throw std::runtime_error("Could not open source file: " + path);
//     }
//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     return buffer.str();
// }
//
// int main(const int argumentCount, char** argumentValues) {
    // if (argumentCount < 2) {
    //     std::cerr << "Usage: edo_compiler <file.edo>" << std::endl;
    //     return 1;
    // }


    // const std::string source = readSourceFile(std::string(argumentValues[1]));
    //
    // Lexer lexer(source);
    // const std::vector<Token> tokens = lexer.tokenize();
    // Parser parser(tokens);
    //
    // while (!parser.isAtEnd()) {
    //     StatementPointer statement = parser.parseStatement();
    //     std::cout << printStatement(*statement);
    // }

    // return 0;
// }