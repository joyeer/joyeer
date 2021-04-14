#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/program.h"
#include "joyeer/runtime/buildin.h"

int main(int argc, char** argv) {
    // command line arguments
    auto options = std::make_shared<CommandLineArguments>(argc, argv);
    
    if(!options->accepted) {
        options->printUsage();
        return -1;
    }
    
    Global::initGlobalTables();
    
    Program program(options);
    program.run(options->inputfile.string());
    
    return 0;
}
