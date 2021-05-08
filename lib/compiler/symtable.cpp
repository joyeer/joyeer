#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/runtime/buildin.h"

std::string debugStringOfSymbolFlag(SymbolFlag flag) {
    switch (flag) {
        case SymbolFlag::varSymbol:
            return "varSymbol";
        case SymbolFlag::funcSymbol:
            return "funcSymbol";
        case SymbolFlag::fieldSymbol:
            return "fieldSymbol";
        case SymbolFlag::constructorSymbol:
            return "constructor";
        case SymbolFlag::typeSymbol:
            return "type";
        case SymbolFlag::fileModuleSymbol:
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
