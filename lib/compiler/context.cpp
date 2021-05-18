#include "joyeer/compiler/context.h"
#include "joyeer/runtime/buildin.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include <cassert>
#include <memory>

CompileContext::CompileContext(CommandLineArguments::Ptr options):
options(options) {
    initializeSymTable();
    initializeGlobalScope();
}

SymbolTable::Ptr CompileContext::initializeSymTable() {
    SymbolTable::Ptr symtable = std::make_shared<SymbolTable>();
    symbols.push_back(symtable);
    return symtable;
}

void CompileContext::finalizeSymTable() {
    symbols.pop_back();
}

SymbolTable::Ptr CompileContext::curSymTable() {
    assert(symbols.size() > 0 );
    return symbols.back();
}

void CompileContext::visit(CompileStage stage, std::function<void ()> visitor) {
    visit(stage, nullptr, visitor);
}

void CompileContext::visit(CompileStage stage, Node::Ptr node, std::function<void()> visit) {
    stages.push_back(stage);
    auto isDeclNode = node->isDeclNode();
    Descriptor::Ptr descriptor = nullptr;
    
    if(isDeclNode) {
        auto declNode = std::static_pointer_cast<DeclNode>(node);
        descriptor = declNode->descriptor;
        descriptors.push(descriptor);
    }
    
    // visit
    visit();
    
    if(isDeclNode) {
        assert(descriptor != nullptr);
        assert(descriptors.top() == descriptor);
        descriptors.pop();
    }
    assert(stages.back() == stage);
    stages.pop_back();
}

void CompileContext::entry(SymbolTable::Ptr table) {
    assert(table != nullptr);
    symbols.push_back(table);
}

void CompileContext::leave(SymbolTable::Ptr table) {
    assert(table != nullptr);
    assert(symbols.back() == table);
    symbols.pop_back();
}

JrType* CompileContext::curType() {
    if(types.size() == 0) {
        return nullptr;
    }
    return types.back();
}

void CompileContext::entry(JrType* type) {
    types.push_back(type);
}

void CompileContext::leave(JrType* type) {
    assert(types.back() == type);
    types.pop_back();
}

JrFunction* CompileContext::curFunction() {
    assert(functions.size() > 0 );
    return functions.back();
}

void CompileContext::entry(JrFunction* function) {
    functions.push_back(function);
}

void CompileContext::leave(JrFunction* function) {
    assert(functions.back() == function);
    functions.pop_back();
}

CompileStage CompileContext::curStage() const {
    assert(stages.size() > 0);
    return stages.back();
}

Symbol::Ptr CompileContext::lookup(const std::string &name) {
    for (auto iterator = symbols.rbegin(); iterator != symbols.rend(); iterator ++) {
        auto symtable = *iterator;
        auto symbol = symtable->find(name);
        if(symbol != nullptr) {
            return symbol;
        }
    }
    
    for(auto iterator = importedSymbols.rbegin(); iterator != importedSymbols.rend(); iterator ++ ) {
        auto symtable = *iterator;
        auto symbol = symtable->find(name);
        if(symbol != nullptr) {
            return symbol;
        }
    }
    
    return nullptr;
}

void CompileContext::associate(JrType* type, SymbolTable::Ptr table) {
    mapOfTypeAndSymbolTable.insert({
        type->addressOfType,
        table
    });
}

SymbolTable::Ptr CompileContext::symtableOfType(JrType* type) {
    return mapOfTypeAndSymbolTable[type->addressOfType];
}

void CompileContext::importSymbolTableOfModule(SymbolTable::Ptr table) {
    importedSymbols.push_back(table);
}

void CompileContext::initializeGlobalScope() {
    
    assert(symbols.size() == 1); // only the global scope
    
    auto table = curSymTable();
    
    for(int index = 0 ; index < Global::functions.size(); index ++) {
        auto func = Global::functions[index];
        auto symbol = std::shared_ptr<Symbol>(new Symbol{
            .name = func->name,
            .flag = SymbolFlag::funcSymbol,
            .addressOfFunc = index,
        });
        table->insert(symbol);
    }
    
    for(int index = 0; index < Global::types.size(); index ++ ) {
        auto type = Global::types[index];
        auto symbol = std::shared_ptr<Symbol>(new Symbol{
            .name = type->name,
            .flag = SymbolFlag::typeSymbol,
            .addressOfType = index
        });
        table->insert(symbol);
    }
}
