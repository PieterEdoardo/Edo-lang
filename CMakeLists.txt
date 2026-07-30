cmake_minimum_required(VERSION 3.20)
project(edo_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(edo_compiler
        src/main.cpp
        src/lexer.cpp
        src/parser.cpp
        src/ast_printer.cpp
)

target_include_directories(edo_compiler PRIVATE src)