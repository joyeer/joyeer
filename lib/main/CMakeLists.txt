
add_joyeer_executable(joyeer 
        main.cpp
        driver.cpp
        )

add_dependencies(joyeer JoyeerRuntime)
add_dependencies(joyeer JoyeerCompiler)
add_dependencies(joyeer JoyeerVM)
add_dependencies(joyeer JoyeerDiagnostic)

target_link_libraries(joyeer
        JoyeerDiagnostic
        JoyeerRuntime
        JoyeerCompiler
        JoyeerVM
)
