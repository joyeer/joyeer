#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>

#include "joyeer/compiler/typedef.h"

enum class SymbolFlag : uint16_t {
    func = 1,
    var,
    field                   = 3,
    class_                  = 5,
    fileModule              = 6,
    staticInitializer       = 7,
    initializer             = 8
};

std::string debugStringOfSymbolFlag(SymbolFlag flag);

/**
 * Symbol contains information about
 * 1. name
 * 2. kind, which kind of symbol, and declared in SymbolFlag
 * 3. Type
 * 4. Address
 */
struct Symbol {
    using Ptr = std::shared_ptr<Symbol>;
public:
    SymbolFlag flag;
    std::string name;

    // address of JrFileModuleType/JrFuncType/JrClassType
    int32_t address = -1;

    // location of field-variable/local-variable inside of Class/FileModule/Func
    int16_t location = -1;

    // create a new symbol
    static Symbol::Ptr make(SymbolFlag flag, const std::string& name, int32_t address, int16_t location = -1) {
        return Symbol::Ptr(new Symbol {
            .flag = flag,
            .name =name,
            .address = address,
            .location = location
        });
    }
};


class SymbolTable {
public:
    typedef std::shared_ptr<SymbolTable> Ptr;
    
public:
    SymbolTable();
    
    // insert an new Symbol
    bool insert(Symbol::Ptr symbol);
    
    // find symbol by a given name
    [[nodiscard]] Symbol::Ptr find(const std::string& name) const;
    
    std::unordered_map<std::string, Symbol::Ptr> symbols;
};

#endif
