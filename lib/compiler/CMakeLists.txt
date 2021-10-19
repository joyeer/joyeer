
add_joyeer_library(joyeerCompiler STATIC
        binder.cpp
        compiler+service.cpp
        context.cpp
        lexparser.cpp
        diagnostic.cpp
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

