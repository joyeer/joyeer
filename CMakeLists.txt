cmake_minimum_required(VERSION 3.16)

project (joyeer
          VERSION 0.0.1)

add_executable(joyeer main.cpp
                      compiler/keyword.cpp
                      compiler/lexer.cpp
                      compiler/token.cpp
                      compiler/syntax/syntax.cpp
                      compiler/AST/statement.cpp
                      compiler/AST/ast.cpp
                      compiler/AST/decl.cpp
                      compiler/AST/expression.cpp
                      compiler/AST/node.cpp
                      runtime/object.cpp)

target_compile_features(joyeer PRIVATE cxx_std_17)
