#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include "compiler/AST/ast.h"
#include "types.h"
#include <unordered_map>
#include <stack>

enum SymbolFlag {
    noneSymbol = 0l,
    blockSymbol = 1L << 1,
    
    sourcFileSymbol = blockSymbol & 1L << 10,
    classTypeSymbol = blockSymbol & 1l << 11,
    varSymbol = 1l << 12
};

struct Symbol {
    typedef std::shared_ptr<Symbol> Pointer;
    
public:
    Symbol(SymbolFlag flag, const std::wstring& name);
    
public:
    TypeDescriptor::Pointer type;
    SymbolFlag flag;
    std::wstring name;
    Node::Pointer node;
};


class SymTable {
public:
    typedef std::shared_ptr<SymTable> Pointer;
public:
    SymTable(std::shared_ptr<SymTable> previous);
    
    // insert an new Symbol
    void insert(Symbol::Pointer symbol);
private:
    SymTable::Pointer previous;
    std::unordered_map<std::wstring, Symbol::Pointer> symbols;
};


class SymbolFactory {
public:
    typedef std::shared_ptr<SymbolFactory> Pointer;
    
public:
    SymbolFactory();
    
    // We create an SymTable for a specific node
    SymTable::Pointer createSymTable(NodePtr node);
    
private:
    std::stack<SymTable::Pointer> stack;
    std::unordered_map<NodePtr, SymTable::Pointer> tables;
     
};

#endif

