#include "driver.h"
#include "joyeer/vm/isolate.h"

Driver::Driver(CommandLineArguments::Ptr arguments):arguments(arguments) {

    vm = new InterpretedIsolatedVM();
    vm->bootstrap();

    compiler = new CompilerService(arguments);
    compiler->strings = vm->strings;
    compiler->types = vm->types;
    compiler->bootstrap();

}

void Driver::run() {
    auto module = compiler->run(arguments->inputfile);
    ((InterpretedIsolatedVM*)vm)->run(module);
}
