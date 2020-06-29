#ifndef __joyeer_runtime_buildin_h__
#define __joyeer_runtime_buildin_h__


#include <string>
#include <unordered_map>
#include <vector>
#include "function.h"

struct Global {
    
    static std::vector<JrFunction::Pointer> funcTable;
    static std::vector<std::wstring> stringTable;
    
    
    /// register an new function in global function table
    static void registerFunction(JrFunction::Pointer func);
};

#endif
