#include "driver.h"

Driver::Driver(CommandLineArguments::Ptr arguments):arguments(arguments) {
    compiler = new CompilerService(arguments);
    isolateVM = new IsolateVM();
}

void Driver::run() {
    auto fileModule = compiler->run(arguments->inputfile);
    isolateVM->run(fileModule);
}
