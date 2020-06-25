
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/object.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/buildin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buildin.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/function.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/function.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/interpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interpreter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/instruction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/instruction.h
)



set(SOURCES ${SOURCES} PARENT_SCOPE)

