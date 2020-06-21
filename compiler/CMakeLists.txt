
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.h )


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/AST)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lexer)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/syntax)


set(SOURCES ${SOURCES} PARENT_SCOPE)

