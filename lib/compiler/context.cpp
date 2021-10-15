#include "joyeer/compiler/context.h"
#include "joyeer/runtime/buildin.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include <cassert>
#include <memory>

CompileContext::CompileContext(CommandLineArguments::Ptr options):
options(options) {
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
    auto isDeclNode = node != nullptr ? node->isDeclNode() : false;
    Descriptor::Ptr descriptor = nullptr;
    
    if(isDeclNode) {
        auto declNode = std::static_pointer_cast<DeclNode>(node);
        descriptor = declNode->descriptor;
        descriptors.push(descriptor);
    }
    
    if(node != nullptr && node->symtable) {
        symbols.push_back(node->symtable);
    }
    
    // visit
    visit();
    
    if(node != nullptr && node->symtable) {
        symbols.pop_back();
    }
    
    if(isDeclNode) {
        assert(descriptor != nullptr);
        assert(descriptors.top() == descriptor);
        descriptors.pop();
    }
    
    assert(stages.back() == stage);
    stages.pop_back();
}

void CompileContext::entry(const SymbolTable::Ptr& table) {
    assert(table != nullptr);
    symbols.push_back(table);
}

void CompileContext::leave(const SymbolTable::Ptr& table) {
    assert(table != nullptr);
    assert(symbols.back() == table);
    symbols.pop_back();
}

JrType* CompileContext::curType() {
    if(types.empty()) {
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
    assert(!functions.empty() );
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
