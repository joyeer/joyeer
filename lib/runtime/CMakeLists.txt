
add_joyeer_library(JoyeerRuntime STATIC
        arguments.cpp
        bytecode.cpp
        descriptor.cpp
        ../diagnostic/diagnostic.cpp
        gc.cpp
        heaps.cpp
        isolate+vm.cpp
        loader.cpp
        memory.cpp
        res+table.cpp
        sys.cpp
        types.cpp
)
