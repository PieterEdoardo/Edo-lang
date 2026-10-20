#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ast_printer.hpp"
#include "lexer.hpp"
#include "parser.hpp"

static std::string readSourceFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open source file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(const int argumentCount, char** argumentValues) {
    if (argumentCount < 2) {
        std::cerr << "Usage: edo_compiler <file.edo>" << std::endl;
        return 1;
    }

    const std::string source = readSourceFile(std::string(argumentValues[1]));

    Lexer lexer(source);
    const std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);

    while (!parser.isAtEnd()) {
        StatementPointer statement = parser.parseStatement();
        std::cout << printStatement(*statement);
    }

    return 0;
}