
add_joyeer_executable(joyeer 
        main.cpp
        driver.cpp
        )

add_dependencies(joyeer JoyeerCommon)
add_dependencies(joyeer JoyeerCompiler)
add_dependencies(joyeer JoyeerVM)

target_link_libraries(joyeer
        JoyeerCommon
        JoyeerCompiler
        JoyeerVM
)
