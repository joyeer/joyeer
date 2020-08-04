#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>
#include <string>
#include <stack>
#include <vector>

enum SymbolFlag {
    
    thisSymbol =        1 << 1,
    funcSymbol =        1 << 2,
    fieldSymbol =       1 << 3,
    constructorSymbol = 1 << 4,
    varSymbol =         1 << 5,
    
    immutableSymbol =   1 << 7,
    
    // this is a type symbol
    typeSymbol =        1 << 10,
    typeUnfixedSymbol =   1 << 11,
    
    moduleSymbol        = 1 << 12,
    
    // delcare & variable symbol
    unfixedMutableVarSymbol =   typeUnfixedSymbol,
    immutableVarSymbol =        varSymbol | immutableSymbol,
    unfixedImmutableVarSymbol = immutableVarSymbol | typeUnfixedSymbol,
    
    // declare field symbols
    mutableFieldSymbol =        fieldSymbol,
    immutableFieldSymbol  =     fieldSymbol | immutableSymbol
    
};

enum SymbolScope: int8_t {
    globalScope     =   1,
    classScope      =   1 << 1,
    functionScope   =   1 << 2,
    fieldScope      =   1 << 3
};

enum SymbolKind: int8_t {
    
};

enum AccessFlag: int8_t {
    publicFlag      =   1,
    protectedFlag   =   1 << 1,
    internalFlag    =   1 << 2,
    privateFlag     =   1 << 3
};

struct Symbol {
    typedef std::shared_ptr<Symbol> Pointer;
public:
    bool isMutable: 1 = true;
    SymbolFlag flag;
    SymbolScope scope;
    
    std::wstring name;
    
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
        int addresOfAnyAssociatedType;
    };
};


class SymbolTable {
public:
    typedef std::shared_ptr<SymbolTable> Pointer;
    typedef std::weak_ptr<SymbolTable> WeakPointer;
    
public:
    SymbolTable();
    
    // insert an new Symbol
    bool insert(Symbol::Pointer symbol);
    
    // find symbol by a given name
    Symbol::Pointer find(const std::wstring& name) const;
    
    // Parent's
    SymbolTable::WeakPointer parent;
    std::vector<SymbolTable::Pointer> children;
    
    std::unordered_map<std::wstring, Symbol::Pointer> symbols;
};



#endif
