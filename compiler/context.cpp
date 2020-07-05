#include "context.h"
#include <cassert>
#include "diagnostic.h"
#include "runtime/buildin.h"

void Scope::insert(Var::Pointer var) {
    if(map.find(var->name) != map.end()) {
        Diagnostics::reportError(L"[Error] duplicated var");
    }
    
    map.insert({
        var->name,
        var
    });
    
    vars.push_back(var);
    var->index = vars.size() - 1;
}

Var::Pointer Scope::find(const std::wstring &name) {
    if(map.find(name) == map.end()) {
        return nullptr;
    }
    return map[name];
}


Var::Pointer ScopeVarFinder::find(const std::wstring& name) {
    for(auto scope: scopes) {
        Var::Pointer variable = scope->find(name);
        if(variable != nullptr) {
            return variable;
        }
    }
    
    return nullptr;
}


CompileContext::CompileContext() {
    initializeScope(globalScope);
}

void CompileContext::initializeScope(ScopeFlag flag) {
    assert(scopes.size() == symbols.size());
    SymTable::Pointer symtable = std::make_shared<SymTable>();
    
    scopes.push_back(flag);
    symbols.push_back(symtable);
}

SymTable::Pointer CompileContext::currentSymbolTable() {
    assert(symbols.size() > 0 );
    return symbols.back();
}

void CompileContext::initializeGlobalScope() {
    
    assert(symbols.size() == 1); // only the global scope
    
    auto table = currentSymbolTable();
    
    for(size_t index = 0 ; index < Global::funcTable.size(); index ++) {
        auto func = Global::funcTable[index];
        auto symbol = std::make_shared<Symbol>(SymbolFlag::funcSymbol, func->name);
        symbol->index = index;
        table->insert(symbol);
    }
}
