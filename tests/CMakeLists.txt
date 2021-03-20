
find_package(Python3 COMPONENTS Interpreter)

if(Python3_Interpreter_FOUND)
  add_test(NAME JoyeerTests COMMAND Python3::Interpreter -m testRunner WORKING_DIRECTORY ${JOYEER_TESTS_DIR})
endif()

