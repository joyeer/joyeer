add_joyeer_executable(joyeer main.cpp)

add_dependencies(joyeer jrRuntime)
add_dependencies(joyeer jrCompiler)

target_link_libraries(joyeer
    jrRuntime
    jrCompiler
)
