
add_joyeer_executable(joyeer 
                        main.cpp
                    )

add_dependencies(joyeer joyeerCommon)
add_dependencies(joyeer joyeerDriver)
#add_dependencies(joyeer joyeerRuntime)
add_dependencies(joyeer joyeerCompiler)

target_link_libraries(joyeer
    joyeerCommon
#    joyeerRuntime
    joyeerCompiler
    joyeerDriver
)
