#include "joyeer/compiler/context.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include <cassert>
#include <memory>
#include <utility>

CompileContext::CompileContext(CommandLineArguments::Ptr options):
options(std::move(options)) {
}

SymbolTable::Ptr CompileContext::curSymTable() {
    assert(!symbols.empty() );
    return symbols.back();
}

void CompileContext::visit(CompileStage stage, const std::function<void ()>& visitor) {
    visit(stage, nullptr, visitor);
}

void CompileContext::visit(CompileStage stage, const Node::Ptr& node, const std::function<void()>& visit) {
    stages.push_back(stage);
    auto isDeclNode = node != nullptr && node->isDeclNode();
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
    
    assert(stage == stages.back());
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


CompileStage CompileContext::curStage() const {
    assert(!stages.empty());
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

JrFuncTypeDef::Ptr CompileContext::curFuncDef() const {
    for (auto iterator = parsingStack.rbegin(); iterator != parsingStack.rend(); iterator ++) {
        auto node = *iterator;
        if(node->typeDef->type == JrTypeType::Function) {
            return std::static_pointer_cast<JrFuncTypeDef>(node->typeDef);
        }
    }

    return nullptr;
}

JrFileModuleTypeDef::Ptr CompileContext::curModuleDef() const {
    return JrFileModuleTypeDef::Ptr();
}
