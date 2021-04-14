#include "joyeer/driver/driver.h"


Driver::Driver(CommandLineArguments::Ptr arguments):arguments(arguments) {
    compiler = new CompilerService(arguments);
}

void Driver::run() {
    compiler->run(arguments->inputfile);
}
