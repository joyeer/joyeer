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
    
    // global strings
    static std::vector<std::wstring> strings;
    
    
    /// register an new function in global function table
    static void registerFunction(JrFunction::Pointer func);
};

#endif
