#ifndef __joyeer_runtime_buildin_h__
#define __joyeer_runtime_buildin_h__

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "runtime.h"
#include "function.h"

struct Global {
    
    // global functions
    static std::vector<JrFunction::Pointer> functions;
    
    // global types
    static std::vector<JrType::Pointer> types;
    
    // global strings
    static std::vector<std::wstring> strings;
    
    // global variables;
    static std::vector<JrVar::Pointer> variables;
    
    /// register an new function in global function table
    static void registerFunction(JrFunction::Pointer func);
    
    /// register an new object type in global table
    static void registerObjectType(JrType::Pointer type);
    
    /// initialize all global tables
    static void initGlobalTables();
};

class TypeTablePrinter {
public:
    TypeTablePrinter(const std::wstring filename);
    void print();
    void print(JrFieldType::Pointer field);
    void print(int addressOfFunc);
    void close();
    
private:
    std::wofstream output;
};

class FunctionTablePrinter {
public:
    FunctionTablePrinter(const std::wstring filename);
    void print();
    void close();
    
private:
    std::wofstream output;
};
#endif
