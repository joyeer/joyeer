#ifndef __joyeer_compiler_sourcefile_h__
#define __joyeer_compiler_sourcefile_h__

#include <filesystem>
#include <string>
#include "joyeer/runtime/object.h"
#include "joyeer/compiler/node+types.h"
#include "joyeer/runtime/sys/module.h"

// SourceFile contains all information of source file in disk
struct SourceFile {
        
    void open(const std::string& path);
    
    JrInt addressOfModuleObj  = -1;
    bool initialized = false;
    
    std::filesystem::path location;
    std::string content;
    
    FileModuleNode::Ptr sourceblock;
    JrModuleClass* moduleClass;
    
    SymbolTable::Ptr exportedSymbolTable = nullptr;
};

#endif
