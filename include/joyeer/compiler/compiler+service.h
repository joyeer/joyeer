#ifndef __joyeer_compiler_compiler_service_h__
#define __joyeer_compiler_compiler_service_h__

#include "joyeer/compiler/context.h"
#include "joyeer/compiler/memory+alignment.h"

class CompilerService : public std::enable_shared_from_this<CompilerService> {
public:
    using Ptr = std::shared_ptr<CompilerService>;

    explicit CompilerService(CommandLineArguments::Ptr options);

    // return the entry file module
    ModuleType::Ptr run(const std::string& inputFile);

    // register a kind
    void declare(const Type::Ptr& type);

    // get a Types base on a given address/position
    Type::Ptr getType(int address);

    Type::Ptr getPrimaryType(ValueType valueType);

    // get the Type declaration of the BuildIns func/object
    Type::Ptr getBuildInsType(BuildIns buildIn) {
        return types[static_cast<int>(buildIn)];
    }

private:

    friend class IRGen;
    friend class IsolateVM;
    friend class MethodResTable;

    // Compile an SourceFile
    ModuleType::Ptr compile(const SourceFile::Ptr& sourcefile);
    // try to import module, and return the exported symtable;
    SourceFile::Ptr tryImport(const CompileContext::Ptr& context, const std::string& moduleName);
    SourceFile::Ptr findSourceFile(const std::string& path, const std::string& relativeFolder = "");
    
    void debugPrint(const Node::Ptr& node, const std::string& debugFilePath);

    // initialize the global symbol table
    void initializeGlobalSymbolTable();

    // initialize the pre-define TypeDefs
    void initializeTypes();

    // register a string into string-table
    intptr_t registerStringResource(const std::string& string) {
        strings.push_back(string);
        return strings.size() - 1;
    }
    
    CommandLineArguments::Ptr options;
    std::unordered_map<std::string, SourceFile::Ptr> sourceFiles;
    
    // global symbols
    SymbolTable::Ptr globalSymbols;

    // Type tables include
    std::vector<Type::Ptr> types;

    // Strings
    std::vector<std::string> strings {};

    // Module static variable memory alignment
    FileModuleMemoryAlignment fileModuleMemoryAlign;
};
#endif
