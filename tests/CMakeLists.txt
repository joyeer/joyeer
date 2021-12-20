
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(Python3_Interpreter_FOUND)

    file(GLOB joyeerTestFiles "basis/*.joyeer")
    foreach(joyeerTest ${joyeerTestFiles})
        add_test(NAME "JoyeerTests - ${joyeerTest}"
                COMMAND Python3::Interpreter -m testRunner "${JOYEER_BIN_OUTPUT_INTDIR}/joyeer" ${joyeerTest}
                WORKING_DIRECTORY ${JOYEER_TESTS_DIR})
    endforeach()
endif()

