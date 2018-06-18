cmake_minimum_required(VERSION 2.8)

project(compiler)
add_executable(${PROJECT_NAME} "main.cpp" "expression.cpp" "function.cpp" "lexer.cpp" "library.cpp" "parser.cpp" "statement.cpp")
