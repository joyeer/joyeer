#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>
#include <string>
#include <stack>
#include <vector>

enum SymbolFlag {
    
    varSymbol =             1,
    funcSymbol =            2,
    fieldSymbol =           3,
    constructorSymbol =     4,
    
    // this is a type symbol
    typeSymbol =            5,
    moduleSymbol =          6
};

std::string debugStringOfSymbolFlag(SymbolFlag flag);

enum SymbolScope: int8_t {
    globalScope     =   1,
    classScope      =   1 << 1,
    functionScope   =   1 << 2,
    fieldScope      =   1 << 3
};

enum AccessFlag: int8_t {
    publicFlag      =   1,
    protectedFlag   =   1 << 1,
    internalFlag    =   1 << 2,
    privateFlag     =   1 << 3
};

struct Symbol {
    typedef std::shared_ptr<Symbol> Ptr;
public:
    bool isMutable: 1 = true;
    bool isTypeFixed: 1 = true;
    bool isExported: 1 = false;
    
    SymbolFlag flag;
    SymbolScope scope;
    
    std::string name;
    
    union {
        // index address of type in Global::types
        int addressOfType = -1;
        // index address of function in Global::types
        int addressOfFunc;
    };
    
    // locate the var symbol's variable position in function
    union {
        int addressOfVariable = -1;
        // the address of the field in Object
        int addressOfField;
    };
 
    // symbols belong to which module, only avalidate in exported symbols
    int addressOfModule = -1;
};


class SymbolTable {
public:
    typedef std::shared_ptr<SymbolTable> Ptr;
    
public:
    SymbolTable();
    
    // insert an new Symbol
    bool insert(Symbol::Ptr symbol);
    
    // find symbol by a given name
    Symbol::Ptr find(const std::string& name) const;
    
    std::unordered_map<std::string, Symbol::Ptr> symbols;
};

#endif