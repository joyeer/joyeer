cmake_minimum_required(VERSION 3.16)

project (joyeer
          VERSION 0.0.1)

add_executable(joyeer main.cpp
                      compiler/keyword.cpp
                      compiler/lexer.cpp
                      compiler/token.cpp
                      runtime/object.cpp)

target_compile_features(joyeer PRIVATE cxx_std_17)
