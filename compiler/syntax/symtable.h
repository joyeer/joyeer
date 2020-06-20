#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include "compiler/AST/ast.h"
#include <unordered_map>
#include <stack>

struct Symbol {
    const static size_t NONE = 0;
    const static size_t CLASS = 1 << 1;
    const static size_t VAR = 1 << 2;
    
    std::wstring name;
    size_t flag;
};
typedef std::shared_ptr<Symbol> SymbolPtr;


class SymTable;
typedef std::shared_ptr<SymTable> SymTablePtr;
class SymTable {
public:
    SymTable(std::shared_ptr<SymTable> previous);
    
private:
    SymTablePtr previous;
    std::unordered_map<std::wstring, SymbolPtr> map;
};


class SymbolFactory {
    
public:
    SymbolFactory();
    // We create an SymTable for a specific node
    SymTablePtr createSymTable(NodePtr node);
    
private:
    std::stack<SymTablePtr> stack;
    
    void append(SymTablePtr table);
};
typedef std::shared_ptr<SymbolFactory> SymbolFactoryPtr;

#endif

