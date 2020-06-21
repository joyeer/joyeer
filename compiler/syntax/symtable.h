#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include "compiler/AST/ast.h"
#include <unordered_map>
#include <stack>

enum SymbolFlag {
    none = 0l,
    block = 1L << 1,
    
    sourcFile = block & 1L << 10,
    class_ = block & 1l << 11,
    var = 1l << 12
};

struct Symbol {
    SymbolFlag flag;
    std::wstring name;
    NodePtr node;
};

typedef std::shared_ptr<Symbol> SymbolPtr;


class SymTable {
public:
    typedef std::shared_ptr<SymTable> Pointer;
public:
    SymTable(std::shared_ptr<SymTable> previous);
    
private:
    SymTable::Pointer previous;
    std::unordered_map<std::wstring, SymbolPtr> map;
};


class SymbolFactory {
    
public:
    SymbolFactory();
    
    // We create an SymTable for a specific node
    SymTable::Pointer createSymTable(NodePtr node);
    
    SymbolPtr createSymbol(const std::wstring& name);
    
private:
    std::stack<SymTable::Pointer> stack;
    std::unordered_map<NodePtr, SymTable::Pointer> map;
    
};
typedef std::shared_ptr<SymbolFactory> SymbolFactoryPtr;

#endif

