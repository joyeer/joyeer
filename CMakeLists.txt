cmake_minimum_required(VERSION 3.16)

project (joyeer
          VERSION 0.0.1)

set(SOURCES main.cpp)

add_subdirectory(compiler)

add_executable(joyeer ${SOURCES})

include_directories(./)

# IDE source group settings
source_group("Source Files\\compiler\\lexer" REGULAR_EXPRESSION "compiler/lexer/.*\\.cpp")
source_group("Source Files\\compiler\\syntax" REGULAR_EXPRESSION "compiler/syntax/.*\\.cpp")
source_group("Source Files\\compiler\\AST" REGULAR_EXPRESSION "compiler/AST/.*\\.cpp")

source_group("Header Files\\compiler\\lexer" REGULAR_EXPRESSION "compiler/lexer/.*\\.h")
source_group("Header Files\\compiler\\syntax" REGULAR_EXPRESSION "compiler/syntax/.*\\.h")
source_group("Header Files\\compiler\\AST" REGULAR_EXPRESSION "compiler/AST/.*\\.h")


target_compile_features(joyeer PRIVATE cxx_std_17)
