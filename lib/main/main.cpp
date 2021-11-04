#include "driver.h"
#include "joyeer/runtime/buildin.h"

int main(int argc, char** argv) {
    // command line arguments
    auto options = std::make_shared<CommandLineArguments>(argc, argv);
    
    if(!options->accepted) {
        options->printUsage();
        return -1;
    }

    Driver driver(options);
    driver.run();
    
    return 0;
}
