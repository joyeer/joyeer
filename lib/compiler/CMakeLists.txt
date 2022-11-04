
add_joyeer_library(JoyeerCompiler STATIC
        compiler+service.cpp
        context.cpp
        lexparser.cpp
        IRGen.cpp
        node.cpp
        node+visitor.cpp
        debugprinter.cpp
        sourcefile.cpp
        symtable.cpp
        syntaxparser.cpp
        token.cpp
        typegen.cpp
        typebinding.cpp
        )

target_link_libraries(JoyeerCompiler
        JoyeerDiagnostic
        JoyeerRuntime
        ${LLVM_LIBS}
)
