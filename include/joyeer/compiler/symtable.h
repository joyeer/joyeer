#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>
#include <string>
#include <stack>
#include <vector>

#include "joyeer/compiler/typedef.h"

enum class SymbolFlag {
    varSymbol =             1,
    funcSymbol =            2,
    fieldSymbol =           3,
    // this is a type symbol
    typeSymbol =            5,
    fileModuleSymbol =      6,
    staticInitializer =     7,
    constructor =           8
};

std::string debugStringOfSymbolFlag(SymbolFlag flag);

struct Symbol {
    using Ptr = std::shared_ptr<Symbol>;
public:
    SymbolFlag flag;
    std::string name;
    
    static Symbol::Ptr make(SymbolFlag flag, const std::string& name) {
        return std::make_shared<Symbol>(Symbol{
            .flag = flag,
            .name =name
        });
    }

    JrTypeDef::Ptr type = nullptr;

    // locate the var symbol's variable position in function
    union {
        int addressOfVariable = -1;
        // the address of the field in Object
        int addressOfField;
    };

};


class SymbolTable {
public:
    typedef std::shared_ptr<SymbolTable> Ptr;
    
public:
    SymbolTable();
    
    // insert an new Symbol
    bool insert(Symbol::Ptr symbol);
    
    // find symbol by a given name
    [[nodiscard]] Symbol::Ptr find(const std::string& name) const;
    
    std::unordered_map<std::string, Symbol::Ptr> symbols;
};

#endif
