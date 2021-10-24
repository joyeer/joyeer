#ifndef __joyeer_runtime_buildin_h__
#define __joyeer_runtime_buildin_h__

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "joyeer/compiler/symtable.h"
#include "joyeer/runtime/runtime.h"
#include "joyeer/runtime/function.h"

struct Global {
    
    // global functions
    static std::vector<JrFunction*> functions;
    
    // global types
    static std::vector<JrType*> types;
    
    // global moudles
    static std::vector<JrModuleClass*> modules;
    
    // global strings
    static std::vector<std::string> strings;

    /// register an new function in global function table
    static void registerFunction(JrFunction* func, JrType* ownerType);
    
    /// register an new object kind in global table
    static void registerObjectType(JrType* type);

    /// initialize all global tables
    static void initGlobalTables();
};

class TypeTablePrinter {
public:
    TypeTablePrinter(const std::string filename);
    void print();
    void print(JrFieldType::Ptr field);
    void print(int addressOfFunc);
    void close();
    
private:
    std::ofstream output;
};

class FunctionTablePrinter {
public:
    FunctionTablePrinter(const std::string filename);
    void print();
    void close();
    
private:
    std::ofstream output;
};
#endif
