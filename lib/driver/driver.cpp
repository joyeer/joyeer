#include "joyeer/driver/driver.h"

Driver::Driver(CommandLineArguments::Ptr arguments):arguments(arguments) {
    compiler = new CompilerService(arguments);
//    runtimeContext = new JrRuntimeContext();
//    interpreter = new JrInterpreter(runtimeContext);
}

void Driver::run() {
    compiler->run(arguments->inputfile);
}
