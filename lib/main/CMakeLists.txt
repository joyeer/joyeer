
add_joyeer_executable(joyeer 
                        main.cpp
                    )

add_dependencies(joyeer JoyeerCommon)
add_dependencies(joyeer JoyeerDriver)
add_dependencies(joyeer JoyeerCompiler)

target_link_libraries(joyeer
    JoyeerCommon
    JoyeerCompiler
    JoyeerDriver
)
