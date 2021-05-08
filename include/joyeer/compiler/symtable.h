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
    fileModuleSymbol =      6
};

std::string debugStringOfSymbolFlag(SymbolFlag flag);

struct Symbol {
    typedef std::shared_ptr<Symbol> Ptr;
public:
    bool isMutable: 1 = true;
    bool isTypeFixed: 1 = true;
    bool isExported: 1 = false;
    
    SymbolFlag flag;
    
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
