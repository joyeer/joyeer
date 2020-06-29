#include "scope.h"
#include "diagnostic.h"

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
