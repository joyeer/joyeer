
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
    ${CMAKE_CURRENT_SOURCE_DIR}/gc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/array.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/primary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/primary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/print.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/print.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/module.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/string.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/string.cpp
)



set(SOURCES ${SOURCES} PARENT_SCOPE)

