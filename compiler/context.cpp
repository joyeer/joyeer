#include "context.h"
#include <cassert>
#include <memory>
#include "symtable.h"
#include "diagnostic.h"
#include "runtime/buildin.h"


CompileContext::CompileContext() {
    initializeScope(globalScope);
    initializeGlobalScope();
}

void CompileContext::initializeScope(ScopeFlag flag) {
    assert(scopes.size() == symbols.size());
    SymbolTable::Pointer symtable = std::make_shared<SymbolTable>();
    
    scopes.push_back(flag);
    symbols.push_back(symtable);
}

void CompileContext::finalizeScope(ScopeFlag flag) {
    assert(scopes.size() == symbols.size());
    
    assert(scopes.back() == flag);
    scopes.pop_back();
    symbols.pop_back();
}

SymbolTable::Pointer CompileContext::curSymTable() {
    assert(symbols.size() > 0 );
    return symbols.back();
}

void CompileContext::visit(CompileStage stage, std::function<void ()> visit) {
    entry(stage);
    visit();
    leave(stage);
}

void CompileContext::entry(CompileStage stage) {
    stages.push_back(stage);
}

void CompileContext::leave(CompileStage stage) {
    assert(stages.size() > 0);
    assert(stages.back() == stage);
    stages.pop_back();
}

CompileStage CompileContext::curStage() const {
    assert(stages.size() > 0);
    return stages.back();
}

Symbol::Pointer CompileContext::lookup(const std::wstring &name) {
    for (auto iterator = symbols.rbegin(); iterator != symbols.rend(); iterator ++) {
        auto symtable = *iterator;
        auto symbol = symtable->find(name);
        if(symbol != nullptr) {
            return symbol;
        }
    }
    
    return nullptr;
}

bool CompileContext::insert(Symbol::Pointer symbol) {
    auto symtable = curSymTable();
    return symtable->insert(symbol);
}

void CompileContext::initializeGlobalScope() {
    
    assert(symbols.size() == 1); // only the global scope
    
    auto table = curSymTable();
    
    for(int index = 0 ; index < Global::functions.size(); index ++) {
        auto func = Global::functions[index];
        auto symbol = std::shared_ptr<Symbol>(new Symbol{
            .name = func->name,
            .flag = SymbolFlag::funcSymbol,
            .index = index,
        });
        table->insert(symbol);
    }
}
