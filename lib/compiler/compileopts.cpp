#include "compileopts.h"
#include <codecvt>
#include <iostream>
#include "compiler/diagnostic.h"

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
    
    // current working directory
    workingDirectory = std::filesystem::current_path();
    
    if(inputfile.is_absolute()) {
        inputfile = workingDirectory / inputfile;
    }
    
    if(std::filesystem::exists(inputfile) == false) {
        Diagnostics::reportError(failure, Diagnostics::errorNoSuchFileOrDirectory);
    }
    
    if(!inputfile.empty()) {
        accepted = true;
    }
}

void CompileOpts::printUsage() {
    std::cout << "Usage: joyeer <inputfile>" << std::endl;
}
