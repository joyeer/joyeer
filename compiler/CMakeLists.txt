
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/IRGen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRGen.h
    ${CMAKE_CURRENT_SOURCE_DIR}/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lexparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lexparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/token.h
    ${CMAKE_CURRENT_SOURCE_DIR}/token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/binder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/binder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/syntaxparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/syntaxparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/symtable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/symtable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/typecheck.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/typecheck.h
    ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context.h
    ${CMAKE_CURRENT_SOURCE_DIR}/program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/program.h
    ${CMAKE_CURRENT_SOURCE_DIR}/compileopts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/compileopts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcefile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcefile.cpp
)

set(SOURCES ${SOURCES} PARENT_SCOPE)

