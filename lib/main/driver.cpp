#include "driver.h"
#include "joyeer/vm/isolate.h"

Driver::Driver(CommandLineArguments::Ptr arguments):arguments(arguments) {
    compiler = new CompilerService(arguments);
}

void Driver::run() {
    auto module = compiler->run(arguments->inputfile);
    isolateVM = new InterpretedIsolatedVM();
}
