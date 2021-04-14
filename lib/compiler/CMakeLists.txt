
add_joyeer_library(joyeerCompiler STATIC
    binder.cpp
    context.cpp
    diagnostic.cpp
    IRGen.cpp
    lexparser.cpp
    node.cpp
    node+types.cpp
    node+visitor.cpp
    node+debugprinter.cpp
    compiler+service.cpp
    sourcefile.cpp
    symtable.cpp
    syntaxparser.cpp
    token.cpp
    typecheck.cpp
)

