#ifndef __joyeer_compiler_sourcefile_h__
#define __joyeer_compiler_sourcefile_h__

#include <filesystem>
#include <string>
#include "joyeer/compiler/node+types.h"
#include "joyeer/runtime/sys/module.h"

// SourceFile contains all information of source file in disk
class SourceFile {
public:
    using Ptr = std::shared_ptr<SourceFile>;
public:
    void open(const std::string& path);
    
    // file location
    std::filesystem::path location;

    // file content
    std::string content;

    // lexer parsing result: token list
    std::vector<Token::Ptr> tokens;

    // grammer parsing result, 
    // filemodule is the root node of AST 
    FileModuleNode::Ptr filemodule;

    JrModuleClass* moduleClass;
    
    SymbolTable::Ptr exportedSymbolTable = nullptr;

};

#endif
