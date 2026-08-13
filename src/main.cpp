#include <algorithm>
#include <iostream>
#include <string>

#include "ast_printer.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main() {
    const std::string source =
    "rax = 2 + 3 * 4 - 1;\n"
    "int* x = malloc(sizeof(int));\n"
    "*x = 55;\n"
    "int* y = &x;\n"
    "char* text = \"Hello World!\";\n"
    "if (rax == 13) {\n"
    "rdi = 4;\n"
    "test(rax, rdi);\n"
    "exit();\n"
    "} else if (rax == 0) {\n"
    "rdi = 6;\n"
    "} else {\n"
    "rdi = 7;\n"
    "}\n";

    Lexer lexer(source);
    const std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);

    while (!parser.isAtEnd()) {
        StatementPointer statement = parser.parseStatement();
        std::cout << printStatement(*statement);
    }

    return 0;
}