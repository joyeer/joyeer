#ifndef __joyeer_compiler_program_h__
#define __joyeer_compiler_program_h__

#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/sourcefile.h"
#include "joyeer/compiler/context.h"

#include <unordered_map>

class Program {
public:
    Program(CompileOpts::Ptr options);
    void run(std::string inputfile);
    
private:
    
    // Compile an SourceFile
    void compile(SourceFile* sourcefile);
    // try to import module, and return the exported symboltable;
    SourceFile* tryImport(CompileContext::Ptr context, const std::string& moduleName);
    SourceFile* findSourceFile(const std::string& path, const std::string relativeFolder = "");
    
    void debugPrint(Node::Ptr node, const std::string& debugFilePath);
    void debugPrint(const std::string& debugFilePath);
    
    CompileOpts::Ptr options;
    std::unordered_map<std::string, SourceFile*> sourcefiles;
};
#endif
