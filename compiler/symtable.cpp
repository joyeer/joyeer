#include "symtable.h"
#include "diagnostic.h"
#include "runtime/buildin.h"

std::wstring debugStringOfSymbolFlag(SymbolFlag flag) {
    switch (flag) {
        case varSymbol:
            return L"varSymbol";
        case funcSymbol:
            return L"funcSymbol";
        case fieldSymbol:
            return L"fieldSymbol";
        case constructorSymbol:
            return L"constructor";
        case typeSymbol:
            return L"type";
        case moduleSymbol:
            return L"module";
    }
}

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
