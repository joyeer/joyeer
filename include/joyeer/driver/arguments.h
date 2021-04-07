#ifndef __joyeer_driver_arguments_h__
#define __joyeer_driver_arguments_h__

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

struct CompileOpts {
    using Ptr = std::shared_ptr<CompileOpts>;
    
    // the executable of joyeer's location
    std::filesystem::path vmLocation;
    // main entry point of source code location
    std::filesystem::path inputfile;
    // working directory for source code
    std::filesystem::path workingDirectory;
    
    bool vmDebug = false;
    bool accepted = false;
    
    CompileOpts(int argc, char** argv);
    
    // print compiler usage
    void printUsage();
    
private:
    void parse(std::vector<std::string>& arguments);
    
    // according to the input file , analyse the working directory
    void parseInputFile(const std::string& inputpath);
};

#endif
