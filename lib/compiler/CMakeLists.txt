
add_joyeer_library(JoyeerCompiler STATIC
        binder.cpp
        compiler+service.cpp
        context.cpp
        lexparser.cpp
        IRGen.cpp
        node.cpp
        node+visitor.cpp
        debugprinter.cpp
        opcode.cpp
        sourcefile.cpp
        symtable.cpp
        syntaxparser.cpp
        token.cpp
        typecheck.cpp
        typedef.cpp
        )

