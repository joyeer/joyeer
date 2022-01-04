#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"
#include "joyeer/runtime/res+table.h"

class CompilerService {
public:
    explicit CompilerService(CommandLineArguments::Ptr options);

    // initialize the pre-define TypeDefs
    void bootstrap();

    // return the entry file module
    ModuleClass* run(const std::string& inputFile);

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
    friend class Driver;

    // Compile an SourceFile
    ModuleClass* compile(const SourceFile::Ptr& sourcefile);

    SourceFile::Ptr findSourceFile(const std::string& path, const std::string& relativeFolder = "");
    
    void debugPrint(const Node::Ptr& node, const std::string& debugFilePath);

    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourceFiles;
    
    // global symbols
    SymbolTable::Ptr globalSymbols = nullptr;

    // resource tables
    TypeResTable* types = nullptr;
    StringResTable* strings = nullptr;

    // the exporting symtable of classes, used to export functions/fields inside a class
    // The first key is the slot of Class
    std::unordered_map<Slot, SymbolTable::Ptr> exportingSymbolTableOfClasses;

};
#endif
