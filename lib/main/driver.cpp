#include "driver.h"
#include "joyeer/vm/isolate.h"

Driver::Driver(Diagnostics* diagnostics, CommandLineArguments::Ptr arguments):arguments(arguments) {

    vm = new InterpretedIsolatedVM();

    compiler = new CompilerService(diagnostics, arguments);
    compiler->strings = vm->strings;
    compiler->types = vm->types;

    compiler->bootstrap();
    vm->bootstrap();
}

void Driver::run() {
    auto module = compiler->run(arguments->inputfile);
    ((InterpretedIsolatedVM*)vm)->run(module);
}
