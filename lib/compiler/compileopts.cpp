#include "joyeer/compiler/compileopts.h"
#include "joyeer/compiler/diagnostic.h"
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
            // input file
            parseInputFile(*iterator);
        }
    }
    
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

void CompileOpts::parseInputFile(const std::string &inputpath) {
    inputfile = std::filesystem::path(inputpath);
    if(inputfile.is_relative()) {
        auto path = std::filesystem::current_path() / inputfile.parent_path();
        workingDirectory = std::filesystem::canonical(path);
    } else {
        // using input file as working directory
        workingDirectory = inputfile.parent_path();
    }
}
