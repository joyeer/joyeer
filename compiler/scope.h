#ifndef __joyeer_compiler_syntax_scope_h__
#define __joyeer_compiler_syntax_scope_h__

#include "types.h"

enum ScopeFlag {
    sourceScope,
    classScope,
    interClassScope,
    methodScope,
    fieldScope,
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
    
    std::vector<Scope::Pointer> scopes;
    
    Var::Pointer find(const std::wstring& name);
};

#endif
