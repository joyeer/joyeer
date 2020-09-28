#include "compileopts.h"
#include <codecvt>
#include <iostream>

CompileOpts::CompileOpts(int argc, char** argv) {
    std::vector<std::string> arguments;
    for(int i = 0 ; i < argc; i ++) {
        arguments.push_back(std::string(argv[i]));
    }
    
    parse(arguments);
}

void CompileOpts::parse(std::vector<std::string>& arguments) {
    std::vector<std::string>::const_iterator iterator = arguments.begin();
    vmLocation = *iterator;
    
    iterator ++;
    for(; iterator != arguments.end(); iterator ++) {
        if(*iterator == "--debug-vm") {
            vmDebug = true;
        } else {
            inputfile = std::filesystem::path(*iterator);
        }
    }
    
    if(!inputfile.empty()) {
        accepted = true;
    }
}

void CompileOpts::printUsage() {
    std::cout << "Usage: joyeer <inputfile>" << std::endl;
}
