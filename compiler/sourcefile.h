#ifndef __joyeer_compiler_sourcefile_h__
#define __joyeer_compiler_sourcefile_h__

#include <filesystem>
#include <string>
#include "runtime/object.h"
#include "compiler/node.h"
#include "runtime/sys/module.h"

// SourceFile contains all information of source file in disk
struct SourceFile {
        
    void open(const std::wstring& path);
    
    JrInt addressOfModuleObj  = -1;
    bool initialized = false;
    
    std::filesystem::path location;
    std::wstring content;
    
    SourceBlock::Ptr sourceblock;
    JrModule* module;
};

#endif
