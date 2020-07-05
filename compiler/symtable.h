#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>
#include <string>
#include <stack>

enum SymbolFlag {
    funcSymbol,
    paramSymbol,
    varSymbol,
    constSymbol
};

struct Symbol {
    typedef std::shared_ptr<Symbol> Pointer;
public:
    Symbol(SymbolFlag flag, const std::wstring& name);
public:
    int index;
    SymbolFlag flag;
    std::wstring name;
};


class SymTable {
public:
    typedef std::shared_ptr<SymTable> Pointer;
public:
    SymTable();
    // insert an new Symbol
    void insert(Symbol::Pointer symbol);
    
    Symbol::Pointer find(const std::wstring& name) const;
    
private:
    std::unordered_map<std::wstring, Symbol::Pointer> symbols;
};

/// SymbolFactory will include global symbols like functions
class SymbolFactory {
public:
    typedef std::shared_ptr<SymbolFactory> Pointer;
    
public:
    SymbolFactory();
    
    // We create an SymTable for a specific node
    SymTable::Pointer createSymTable();
    
    SymTable::Pointer getGlobalSymbolTable();
    
private:
    void createGlobalSymbolTable();
private:
    SymTable::Pointer globalSymbolTable;
};

#endif
