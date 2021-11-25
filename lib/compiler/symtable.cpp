#include "joyeer/compiler/symtable.h"
#include "joyeer/common/diagnostic.h"

std::string debugStringOfSymbolFlag(SymbolFlag flag) {
    switch (flag) {
        case SymbolFlag::var:
            return "var";
        case SymbolFlag::func:
            return "func";
        case SymbolFlag::field:
            return "field";
        case SymbolFlag::klass:
            return "class";
        default:
            assert(false);
    }
}

/////////////////////////////////////////////////////////////////
// SymTable
/////////////////////////////////////////////////////////////////
SymbolTable::SymbolTable() :
symbols() {
}

bool SymbolTable::insert(const Symbol::Ptr& symbol) {
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
