#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"

#include <unordered_map>


class CompilerService : public std::enable_shared_from_this<CompilerService> {
public:
    explicit CompilerService(CommandLineArguments::Ptr options);
    void run(const std::string& inputFile);
    
    // declare a DeclNode
    void declare(DeclNode::Ptr decl);

private:
    
    // Compile an SourceFile
    void compile(const SourceFile::Ptr& sourcefile);
    // try to import module, and return the exported symtable;
    SourceFile::Ptr tryImport(const CompileContext::Ptr& context, const std::string& moduleName);
    SourceFile::Ptr findSourceFile(const std::string& path, const std::string& relativeFolder = "");
    
    void debugPrint(const Node::Ptr& node, const std::string& debugFilePath);
    void debugPrint(const std::string& debugFilePath);
    
    // initialize the global symbol table
    void initializeGlobalSymbolTable();
    
    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourceFiles;
    
    // global symbols
    SymbolTable::Ptr globalSymbols;

};
#endif
