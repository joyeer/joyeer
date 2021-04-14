#ifndef __joyeer_compiler_program_h__
#define __joyeer_compiler_program_h__

#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/context.h"

#include <unordered_map>

class Program {
public:
    Program(CommandLineArguments::Ptr options);
    void run(std::string inputfile);
    
private:
    
    // Compile an SourceFile
    void compile(SourceFile::Ptr sourcefile);
    // try to import module, and return the exported symboltable;
    SourceFile::Ptr tryImport(CompileContext::Ptr context, const std::string& moduleName);
    SourceFile::Ptr findSourceFile(const std::string& path, const std::string relativeFolder = "");
    
    void debugPrint(Node::Ptr node, const std::string& debugFilePath);
    void debugPrint(const std::string& debugFilePath);
    
    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourcefiles;
};
#endif
