#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"
#include "joyeer/compiler/compiler+request.h"
#include "joyeer/compiler/symbol+repo.h"

#include <unordered_map>


class CompilerService {
public:
    CompilerService(CommandLineArguments::Ptr options);
    void run(std::string inputfile);
    
    // declare a File Module
    void declare(FileModuleNode::Ptr filemodule);

    void query(const std::string& descriptor);
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
    
    // All the delcaration descriptors include FileModule/Class/Node
    std::unordered_map<std::string, Node::Ptr> descriptors;
};
#endif
