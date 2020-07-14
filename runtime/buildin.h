#ifndef __joyeer_runtime_buildin_h__
#define __joyeer_runtime_buildin_h__


#include <string>
#include <unordered_map>
#include <vector>
#include "runtime.h"
#include "function.h"

struct Global {
    
    // global variables
    static std::vector<JrVar::Pointer> variables;
    
    // global functions
    static std::vector<JrFunction::Pointer> functions;
    
    // global types
    static std::vector<JrType::Pointer> types;
    
    // global strings
    static std::vector<std::wstring> strings;
    
    /// register an new function in global function table
    static void registerFunction(JrFunction::Pointer func);
    
    /// register an new object type in global table
    static void registerObjectType(JrObjectType::Pointer type);
    
    /// initialize all global tables
    static void initGlobalTables();
};

#endif
