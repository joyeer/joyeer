
add_executable(UnitTests 
    main.cpp
    common_descriptor_test.cpp
)

TARGET_LINK_LIBRARIES(UnitTests
    libgtest
    libgmock
    JoyeerCommon
    JoyeerCompiler
    JoyeerDriver
)

add_test(NAME UnitTests COMMAND UnitTests)

add_dependencies(joyeer UnitTests)