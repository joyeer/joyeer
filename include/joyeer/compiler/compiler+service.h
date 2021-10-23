#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"

class CompilerService : public std::enable_shared_from_this<CompilerService> {
public:
    using Ptr = std::shared_ptr<CompilerService>;

    explicit CompilerService(CommandLineArguments::Ptr options);
    void run(const std::string& inputFile);

    // register a type
    void declare(const JrTypeDef::Ptr& type);

    // get a TypeDef from an address
    JrTypeDef::Ptr getTypeDefBy(int address);
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

    // initialize the pre-define TypeDefs
    void initializeTypeDefs();
    
    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourceFiles;
    
    // global symbols
    SymbolTable::Ptr globalSymbols;

    // Type tables include
    std::vector<JrTypeDef::Ptr> types;
};
#endif
