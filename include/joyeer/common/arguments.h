#ifndef __joyeer_common_arguments_h__
#define __joyeer_common_arguments_h__

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

struct CommandLineArguments {
    using Ptr = std::shared_ptr<CommandLineArguments>;
    
    // the executable of joyeer's locationInParent
    std::filesystem::path vmLocation;
    // main entry point of source code locationInParent
    std::filesystem::path inputfile;
    // working directory for source code
    std::filesystem::path workingDirectory;
    
    bool vmDebug = true;
    bool accepted = false;
    
    CommandLineArguments(int argc, char** argv);
    
    // print compiler usage
    static void printUsage();
    
private:
    void parse(std::vector<std::string>& arguments);
    
    // according to the input file , analyse the working directory
    void parseInputFile(const std::string& inputpath);
};

#endif
