
add_joyeer_executable(joyeer 
                        main.cpp
                        driver.cpp
                    )

add_dependencies(joyeer joyeerRuntime)
add_dependencies(joyeer joyeerCompiler)

target_link_libraries(joyeer
    joyeerRuntime
    joyeerCompiler
)
