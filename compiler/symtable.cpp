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
    
    return false;
}

Symbol::Pointer SymbolTable::find(const std::wstring& name) const {
    auto result = symbols.find(name);
    if( result != symbols.end()) {
        return result->second;
    }
    
    return nullptr;
}

std::vector<Symbol::Pointer> SymbolTable::allVarSymbols() const {
    std::vector<Symbol::Pointer> result;
    for(auto symbol: symbols) {
        if((symbol.second->flag & declSymbol) == declSymbol) {
            result.push_back(symbol.second);
        }
    }
    
    for(auto table: children) {
        auto symbols = table->allVarSymbols();
        result.insert(result.end(), std::begin(symbols), std::end(symbols));
    }
    return result;
}
