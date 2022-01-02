#include "driver.h"

Driver::Driver(CommandLineArguments::Ptr arguments):arguments(arguments) {
    compiler = new CompilerService(arguments);
}

void Driver::run() {
    auto fileModule = compiler->run(arguments->inputfile);
    isolateVM = new IsolateVM(compiler);
    isolateVM->run(fileModule);
}
