#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <string>
#include <unordered_map>
#include <stack>

struct Symbol {
    const static size_t NONE = 0;
    const static size_t CLASS = 1 << 1;
    const static size_t VAR = 1 << 2;
    
    std::wstring name;
    size_t flag;
};


class SymTable {

    std::shared_ptr<SymTable> previous;
    std::unordered_map<std::wstring, std::shared_ptr<SymTable>> map;
    
    SymTable(std::shared_ptr<SymTable> previous);
};


class SymTableStack {
    
    std::stack<SymTable> stack;
    
    void append(std::shared_ptr<SymTable> table);
    
};

#endif

