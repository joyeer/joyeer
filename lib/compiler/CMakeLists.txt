
add_joyeer_library(joyeerCompiler STATIC
        lexparser.cpp
        binder.cpp
        context.cpp
        diagnostic.cpp
        IRGen.cpp
        lexparser.cpp
        node.cpp
        node+visitor.cpp
        debugprinter.cpp
        compiler+service.cpp
        sourcefile.cpp
        symtable.cpp
        syntaxparser.cpp
        token.cpp
        typecheck.cpp
        )

