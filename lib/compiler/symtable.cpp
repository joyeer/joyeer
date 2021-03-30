#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/runtime/buildin.h"

std::string debugStringOfSymbolFlag(SymbolFlag flag) {
    switch (flag) {
        case varSymbol:
            return "varSymbol";
        case funcSymbol:
            return "funcSymbol";
        case fieldSymbol:
            return "fieldSymbol";
        case constructorSymbol:
            return "constructor";
        case typeSymbol:
            return "type";
        case moduleSymbol:
            return "module";
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

Symbol::Ptr SymbolTable::find(const std::string& name) const {
    auto result = symbols.find(name);
    if( result != symbols.end()) {
        return result->second;
    }
    
    return nullptr;
}
