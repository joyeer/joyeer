#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/runtime/buildin.h"

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

bool SymbolTable::insert(Symbol::Ptr symbol) {
    if(symbols.find(symbol->name) != symbols.end()) {
        return false;
    }
    
    symbols.insert({
        symbol->name,
        symbol
    });
    
    return true;
}

Symbol::Ptr SymbolTable::find(const std::wstring& name) const {
    auto result = symbols.find(name);
    if( result != symbols.end()) {
        return result->second;
    }
    
    return nullptr;
}
