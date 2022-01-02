#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"
#include "joyeer/runtime/res+table.h"

class CompilerService : public std::enable_shared_from_this<CompilerService> {
public:
    using Ptr = std::shared_ptr<CompilerService>;

    explicit CompilerService(CommandLineArguments::Ptr options);

    // return the entry file module
    ModuleType* run(const std::string& inputFile);

    // register a kind
    int declare(Type* type);

    // get a Types base on a given typeSlot/position
    Type* getType(int address);

    Type* getType(ValueType valueType);

    Type* getType(BuildIns buildIn);

    // get specific type's exporting symbol table
    SymbolTable::Ptr getExportingSymbolTable(int typeSlot);

private:

    friend class IRGen;
    friend class IsolateVM;

    // Compile an SourceFile
    ModuleType* compile(const SourceFile::Ptr& sourcefile);

    SourceFile::Ptr findSourceFile(const std::string& path, const std::string& relativeFolder = "");
    
    void debugPrint(const Node::Ptr& node, const std::string& debugFilePath);

    // initialize the pre-define TypeDefs
    void initializeTypes();

    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourceFiles;
    
    // global symbols
    SymbolTable::Ptr globalSymbols;

    TypeResTable* types { new TypeResTable() };
    StringResTable* strings { new StringResTable() };

    // the exporting symtable of classes, used to export functions/fields inside a class
    // The first key is the slot of ClassType
    std::unordered_map<int, SymbolTable::Ptr> exportingSymbolTableOfClasses;

};
#endif
