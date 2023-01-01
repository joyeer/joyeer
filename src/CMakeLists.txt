set(SOURCES
        driver.cpp
        main.cpp
        compiler/compiler+service.cpp
        compiler/context.cpp
        compiler/lexparser.cpp
        compiler/IRGen.cpp
        compiler/node.cpp
        compiler/node+visitor.cpp
        compiler/debugprinter.cpp
        compiler/sourcefile.cpp
        compiler/symtable.cpp
        compiler/syntaxparser.cpp
        compiler/token.cpp
        compiler/typegen.cpp
        compiler/typebinding.cpp
        diagnostic/diagnostic.cpp
        runtime/arguments.cpp
        runtime/bytecode.cpp
        runtime/descriptor.cpp
        runtime/gc.cpp
        runtime/heaps.cpp
        runtime/isolate+vm.cpp
        runtime/loader.cpp
        runtime/memory.cpp
        runtime/res+table.cpp
        runtime/sys.cpp
        runtime/types.cpp
        vm/frame.cpp
        vm/interpreter.cpp
        vm/isolate.cpp)

add_executable(joyeer ${SOURCES})

