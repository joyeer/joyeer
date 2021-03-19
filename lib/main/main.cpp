#include "joyeer/compiler/program.h"
#include "joyeer/runtime/buildin.h"

int main(int argc, char** argv) {
    // command line arguments
    auto options = std::make_shared<CompileOpts>(argc, argv);
    
    if(!options->accepted) {
        options->printUsage();
        return -1;
    }
    
    Global::initGlobalTables();
    
    Program program(options);
    program.run(options->inputfile.wstring());
    
    return 0;
}
