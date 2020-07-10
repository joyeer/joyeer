#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>
#include <string>
#include <stack>
#include <vector>

enum SymbolFlag {
    declSymbol =        1,
    refSymbol =         1 << 1,
    classSymbol =       1 << 2,
    funcSymbol =        1 << 3,
    varSymbol =         1 << 4,
    mutableSymbol =     1 << 5,
    immutableSymbol =   1 << 6,
    
    // this is a type symbol
    typeSymbol =        1 << 10,
    typeUnfixedSymbol =   1 << 11,
    
    // delcare & variable symbol
    mutableVarSymbol =          declSymbol | varSymbol | mutableSymbol,
    unfixedMutableVarSymbol =   mutableVarSymbol | typeUnfixedSymbol,
    immutableVarSymbol =        declSymbol | varSymbol | immutableSymbol,
    unfixedImmutableVarSymbol = immutableVarSymbol | typeUnfixedSymbol,
    
};


struct Symbol {
    typedef std::shared_ptr<Symbol> Pointer;
public:
    SymbolFlag flag;
    std::wstring name;
    union {
        int index;
        int addressOfType;
        int addressOfFunc;
        int addressOfClass;
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
    
    std::vector<Symbol::Pointer> allVarSymbols() const;

    // Parent's
    SymbolTable::WeakPointer parent;
    std::vector<SymbolTable::Pointer> children;
    
private:
    std::unordered_map<std::wstring, Symbol::Pointer> symbols;
};



#endif
