
add_joyeer_library(joyeerCompiler STATIC
    lexparser.cpp
    binder.cpp
    context.cpp
    diagnostic.cpp
    IRGen.cpp
    lexparser.cpp
    node.cpp
    node+types.cpp
    node+visitor.cpp
    node+stmt.cpp
    node+expr.cpp
    node+debugprinter.cpp
    compiler+service.cpp
    sourcefile.cpp
    symtable.cpp
    syntaxparser.cpp
    token.cpp
    typecheck.cpp
)

