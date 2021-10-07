#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"
#include "joyeer/compiler/symbol+repo.h"

#include <unordered_map>


class CompilerService : public std::enable_shared_from_this<CompilerService> {
public:
    CompilerService(CommandLineArguments::Ptr options);
    void run(std::string inputfile);
    
    // declare a DeclNode
    void declare(DeclNode::Ptr decl);
    
    void query(const std::string& descriptor);
private:
    
    // Compile an SourceFile
    void compile(SourceFile::Ptr sourcefile);
    // try to import module, and return the exported symboltable;
    SourceFile::Ptr tryImport(CompileContext::Ptr context, const std::string& moduleName);
    SourceFile::Ptr findSourceFile(const std::string& path, const std::string& relativeFolder = "");
    
    void debugPrint(Node::Ptr node, const std::string& debugFilePath);
    void debugPrint(const std::string& debugFilePath);
    
    // intialize the global symbol table
    void initializeGlobalSymbolTable();
    
    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourcefiles;
    
    // global symbols
    SymbolTable::Ptr globalSymbols;
    
    // All the declaration descriptors include FileModule/Class/Node
    NodeRepository::Ptr repository = std::make_shared<NodeRepository>();
};
#endif
