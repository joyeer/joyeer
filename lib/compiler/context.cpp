#include "joyeer/compiler/context.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/common/diagnostic.h"
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

    if(node != nullptr && node->getType() != nullptr && (node->isDeclNode() || node->kind == SyntaxKind::stmtsBlock)) {
        types.push_back(node->getType());
    }

    // visit
    visit();

    if(node != nullptr && node->symtable) {
        symbols.pop_back();
    }

    if(node != nullptr && node->getType() != nullptr && (node->isDeclNode() || node->kind == SyntaxKind::stmtsBlock)) {
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

JrType::Ptr CompileContext::curTypeDef() const {
    return types.back();
}

JrType::Ptr CompileContext::curDeclTypeDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->kind == JrTypeKind::Function ||
            typeDef->kind == JrTypeKind::Class ||
            typeDef->kind == JrTypeKind::FileModule ) {
            return std::static_pointer_cast<JrBlockType>(typeDef);
        }
    }
    return nullptr;
}

JrBlockType::Ptr CompileContext::curBlockDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->kind == JrTypeKind::Block) {
            return std::static_pointer_cast<JrBlockType>(typeDef);
        }
    }
    return nullptr;
}

JrFuncType::Ptr CompileContext::curFuncDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->kind == JrTypeKind::Function) {
            return std::static_pointer_cast<JrFuncType>(typeDef);
        }
    }

    return nullptr;
}

JrFileModuleType::Ptr CompileContext::curModuleDef() const {
    // The top one must be the FileModuleTypeDef
    return std::static_pointer_cast<JrFileModuleType>(types[0]);
}
