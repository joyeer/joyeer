cmake_minimum_required(VERSION 3.16)

project (joyeer
          VERSION 0.0.1)


set(SOURCES main.cpp)

add_subdirectory(compiler)
add_subdirectory(runtime)

add_executable(joyeer ${SOURCES})

include_directories(./)

# IDE source group settings
source_group("compiler" REGULAR_EXPRESSION "compiler/.*\\.(cpp|h)")
source_group("runtime" REGULAR_EXPRESSION "runtime/.*\\.(cpp|h)")
source_group("runtime\\sys" REGULAR_EXPRESSION "runtime/sys/.*\\.(cpp|h)")


target_compile_features(joyeer PRIVATE cxx_std_20)
set_property(TARGET joyeer PROPERTY CXX_STANDARD 20)
