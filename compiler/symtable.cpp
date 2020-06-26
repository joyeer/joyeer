#include "symtable.h"
#include "diagnostic.h"
#include "runtime/buildin.h"

/////////////////////////////////////////////////////////////////
// Symbol
/////////////////////////////////////////////////////////////////

Symbol::Symbol(SymbolFlag flag, const std::wstring& name):
flag(flag),
name(name),
index(-1) {
}

/////////////////////////////////////////////////////////////////
// SymTable
/////////////////////////////////////////////////////////////////
SymTable::SymTable() :
symbols() {
}

void SymTable::insert(Symbol::Pointer symbol) {
    if(symbols.find(symbol->name) != symbols.end()) {
        Diagnostics::reportError(L"Duplicate symbol" + symbol->name);
        return;
    }
    symbols[symbol->name] = symbol;
}

Symbol::Pointer SymTable::find(const std::wstring& name) const {
    auto result = symbols.find(name);
    if( result != symbols.end()) {
        return result->second;
    }
    
    return nullptr;
}

/////////////////////////////////////////////////////////////////
// SymbolFactory
/////////////////////////////////////////////////////////////////

SymbolFactory::SymbolFactory() {
    createGlobalSymbolTable();
}

SymTable::Pointer SymbolFactory::createSymTable() {
    auto pointer = std::make_shared<SymTable>();
    return pointer;
}

SymTable::Pointer SymbolFactory::getGlobalSymbolTable() {
    return globalSymbolTable;
}

void SymbolFactory::createGlobalSymbolTable() {
    auto table = createSymTable();
    
    for(size_t index = 0 ; index < Global::funcTable.size(); index ++) {
        auto func = Global::funcTable[index];
        auto symbol = std::make_shared<Symbol>(SymbolFlag::funcSymbol, func->name);
        symbol->index = index;
        
        table->insert(symbol);
    }
    
    globalSymbolTable = table;
}
