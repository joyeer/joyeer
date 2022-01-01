#include "joyeer/compiler/context.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/compiler+service.h"
#include "joyeer/runtime/diagnostic.h"
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

Type::Ptr CompileContext::curDeclType() const {
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

BlockType::Ptr CompileContext::curBlockType() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeSlot = *iterator;
        auto type = compiler->getType(typeSlot);
        if(type->kind == ValueType::Block) {
            return std::static_pointer_cast<BlockType>(type);
        }
    }
    return nullptr;
}

FuncType::Ptr CompileContext::curFuncType() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeSlot = *iterator;
        auto type = compiler->getType(typeSlot);
        if(type->kind == ValueType::Func) {
            return std::static_pointer_cast<FuncType>(type);
        }
    }

    return nullptr;
}

ModuleType::Ptr CompileContext::curModuleType() const {
    auto type = compiler->getType(types[0]);
    assert(type->kind == ValueType::Module);
    return std::static_pointer_cast<ModuleType>(type);
}
