#ifndef __joyeer_compiler_compileopts_h__
#define __joyeer_compiler_compileopts_h__

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

class CompileOpts {
public:
    typedef std::shared_ptr<CompileOpts> Ptr;
    
    CompileOpts(int argc, char** argv);
    
    // print compiler usage
    void printUsage();
    
    // the executable of joyeer's location
    std::filesystem::path vmLocation;
    // main entry point of source code location
    std::filesystem::path inputfile;
    
    bool vmDebug = false;
    bool accepted = false;
    
private:
    void parse(std::vector<std::string>& arguments);
};


#endif
