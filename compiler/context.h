#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "symtable.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

enum ScopeFlag {
    globalScope,
    sourceScope,
    classScope,
    funcScope,
    codeScope,
};

struct Var {
    typedef std::shared_ptr<Var> Pointer;
    
    bool isMutable;
    std::wstring name;
    int index;
};

struct Scope {
    typedef std::shared_ptr<Scope> Pointer;
    
    ScopeFlag flag;
    std::vector<Var::Pointer> vars;
    std::unordered_map<std::wstring, Var::Pointer> map;
    
    void insert(Var::Pointer var);
    
    Var::Pointer find(const std::wstring& name);
    
};

struct ScopeVarFinder {
    typedef std::shared_ptr<ScopeVarFinder> Pointer;
    
    std::vector<Scope::Pointer> scopes;
    
    Var::Pointer find(const std::wstring& name);
};


class CompileContext {
    
public:
    CompileContext();
    
    void initializeScope(ScopeFlag flag);
    void finalizeScope();
    
    std::pair<Symbol::Pointer, ScopeFlag> lookup(const std::wstring& name);
    
    SymTable::Pointer currentSymbolTable();
    
protected:
    // initializ global scope, e.g. buildin functions/object etc
    void initializeGlobalScope();
protected:
    std::vector<SymTable::Pointer> symbols;
    std::vector<ScopeFlag> scopes;
};

#endif

