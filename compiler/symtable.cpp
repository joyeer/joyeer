#include "symtable.h"
#include "diagnostic.h"
#include "runtime/buildin.h"

/////////////////////////////////////////////////////////////////
// SymTable
/////////////////////////////////////////////////////////////////
SymbolTable::SymbolTable() :
symbols() {
}

bool SymbolTable::insert(Symbol::Pointer symbol) {
    if(symbols.find(symbol->name) != symbols.end()) {
        return false;
    }
    
    symbols.insert({
        symbol->name,
        symbol
    });
    
    return true;
}

Symbol::Pointer SymbolTable::find(const std::wstring& name) const {
    auto result = symbols.find(name);
    if( result != symbols.end()) {
        return result->second;
    }
    
    return nullptr;
}
