#include "driver.h"
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/CommandLine.h>

int main(int argc, char** argv) {

    llvm::InitLLVM X(argc, argv);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Joyeer");

    auto diagnostics = new Diagnostics();
    // command line arguments
    auto options = std::make_shared<CommandLineArguments>(diagnostics, argc, argv);
    
    if(!options->accepted) {
        options->printUsage();
        return 0;
    }

    Driver driver(diagnostics, options);
    driver.run();
    
    return 0;
}
