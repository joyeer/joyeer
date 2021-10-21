#ifndef __joyeer_compiler_syntax_symtable_h__
#define __joyeer_compiler_syntax_symtable_h__

#include <unordered_map>
#include <string>
#include <stack>
#include <vector>

#include "joyeer/compiler/typedef.h"

enum class SymbolFlag {
    var                     = 1,
    func                    = 2,
    field                   = 3,
    class_                  = 5,
    fileModule              = 6,
    staticInitializer       = 7,
    initializer             = 8
};

std::string debugStringOfSymbolFlag(SymbolFlag flag);

/**
 * Symbol contains information about
 * 1. name
 * 2. kind, which kind of symbol, and declared in SymbolFlag
 * 3. Type
 * 4. Address
 */
struct Symbol {
    using Ptr = std::shared_ptr<Symbol>;
public:
    SymbolFlag flag;
    std::string name;

    JrTypeDef::Ptr type;
    int address = -1;

    static Symbol::Ptr make(SymbolFlag flag, const std::string& name) {
        return std::make_shared<Symbol>( Symbol {
            .flag = flag,
            .name =name
        });
    }
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
