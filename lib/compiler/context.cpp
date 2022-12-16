#include "joyeer/compiler/context.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/compiler+service.h"
#include "joyeer/diagnostic/diagnostic.h"
#include <cassert>
#include <memory>
#include <utility>

CompileContext::CompileContext(Diagnostics* diagnostics, const SymbolTable::Ptr& globalSymTable){
    symbols.push_back(globalSymTable);
    this->diagnostics = diagnostics;
}

SymbolTable::Ptr CompileContext::curSymTable() {
    assert(!symbols.empty() );
    return symbols.back();
}

void CompileContext::visit(CompileStage stage, const Node::Ptr& node, const std::function<void()>& visit) {
    stages.push_back(stage);

    if(node->isStmtBlock()) {
        scopes.push_back(std::dynamic_pointer_cast<StmtsBlock>(node));
    }

    if(node != nullptr && node->symtable) {
        symbols.push_back(node->symtable);
    }

    if(node != nullptr && node->typeSlot != -1 && (node->isDeclNode() || node->kind == SyntaxKind::stmtsBlock)) {
        types.push_back(node->typeSlot);
    }

    // visit
    visit();

    if(node != nullptr && node->symtable) {
        symbols.pop_back();
    }

    if(node != nullptr && node->typeSlot != -1 && (node->isDeclNode() || node->kind == SyntaxKind::stmtsBlock)) {
        types.pop_back();
    }

    if(node->isStmtBlock()) {
        scopes.pop_back();
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

Type* CompileContext::curDeclType() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeSlot = *iterator;
        auto type = compiler->getType(typeSlot);
        if(type->kind == ValueType::Func ||
            type->kind == ValueType::Class ||
            type->kind == ValueType::Module ) {
            return type;
        }
    }
    return nullptr;
}

Function* CompileContext::curFuncType() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeSlot = *iterator;
        auto type = compiler->getType(typeSlot);
        if(type->kind == ValueType::Func) {
            return (Function*)(type);
        }
    }

    return nullptr;
}

Class* CompileContext::curClassType() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeSlot = *iterator;
        auto type = compiler->getType(typeSlot);
        if(type->kind == ValueType::Class) {
            return (Class*)(type);
        }
    }

    return nullptr;
}
