#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>

#include "joyeer/compiler/typedef.h"

enum class SymbolFlag : uint16_t {
    func = 1,
    var,
    field,
    klass,d
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

    SymbolFlag flag;
    std::string name;

    // address of ModuleType/FuncType/ClassType
    int32_t address = -1;

    Symbol(SymbolFlag flag, std::string  name, int address);

};


class SymbolTable {
public:
    typedef std::shared_ptr<SymbolTable> Ptr;
    
public:
    SymbolTable();
    
    // insert an new Symbol
    bool insert(const Symbol::Ptr& symbol);
    
    // find symbol by a given name
    [[nodiscard]] Symbol::Ptr find(const std::string& name) const;
    
    std::unordered_map<std::string, Symbol::Ptr> symbols;
};

#endif
