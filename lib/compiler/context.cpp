#include "joyeer/compiler/context.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/diagnostic.h"
#include <cassert>
#include <memory>
#include <utility>

CompileContext::CompileContext(CommandLineArguments::Ptr options, const SymbolTable::Ptr& globalSymTable):
options(std::move(options)) {
    symbols.push_back(globalSymTable);
}

SymbolTable::Ptr CompileContext::curSymTable() {
    assert(!symbols.empty() );
    return symbols.back();
}

void CompileContext::visit(CompileStage stage, const Node::Ptr& node, const std::function<void()>& visit) {
    stages.push_back(stage);

    if(node != nullptr && node->symtable) {
        symbols.push_back(node->symtable);
    }

    if(node != nullptr && node->getTypeDef() && node->isDeclNode()) {
        types.push_back(node->getTypeDef());
    }

    // visit
    visit();

    if(node != nullptr && node->symtable) {
        symbols.pop_back();
    }

    if(node != nullptr && node->getTypeDef() && node->isDeclNode()) {
        types.pop_back();
    }

    assert(stage == stages.back());
    stages.pop_back();
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

JrBlockTypeDef::Ptr CompileContext::curBlockDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->type == JrTypeType::Block) {
            return std::static_pointer_cast<JrBlockTypeDef>(typeDef);
        }
    }
    return nullptr;
}

JrFuncTypeDef::Ptr CompileContext::curFuncDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->type == JrTypeType::Function) {
            return std::static_pointer_cast<JrFuncTypeDef>(typeDef);
        }
    }

    return nullptr;
}

JrFileModuleTypeDef::Ptr CompileContext::curModuleDef() const {
    return JrFileModuleTypeDef::Ptr();
}
