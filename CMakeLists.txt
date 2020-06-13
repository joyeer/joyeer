cmake_minimum_required(VERSION 3.16)

project (joyeer
          VERSION 0.0.1)

set(SOURCES main.cpp
            )

add_subdirectory(compiler)

add_executable(joyeer ${SOURCES})

include_directories(./)

# IDE source group settings
source_group("compiler\\lexer" REGULAR_EXPRESSION "compiler/lexer/.*\\.(cpp|h)")
source_group("compiler\\syntax" REGULAR_EXPRESSION "compiler/syntax/.*\\.(cpp|h)")
source_group("compiler\\AST" REGULAR_EXPRESSION "compiler/AST/.*\\.(cpp|h)")


target_compile_features(joyeer PRIVATE cxx_std_17)
