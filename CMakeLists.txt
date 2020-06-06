cmake_minimum_required(VERSION 3.16)
project (joyeer)

add_executable(joyeer main.cpp)
target_compile_features(joyeer PRIVATE cxx_std_17)