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

Type::Ptr CompileContext::curTypeDef() const {
    return types.back();
}

Type::Ptr CompileContext::curDeclTypeDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->kind == TypeKind::Function ||
           typeDef->kind == TypeKind::Class ||
           typeDef->kind == TypeKind::FileModule ) {
            return std::static_pointer_cast<BlockType>(typeDef);
        }
    }
    return nullptr;
}

BlockType::Ptr CompileContext::curBlockDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->kind == TypeKind::Block) {
            return std::static_pointer_cast<BlockType>(typeDef);
        }
    }
    return nullptr;
}

FuncType::Ptr CompileContext::curFuncDef() const {
    for (auto iterator = types.rbegin(); iterator != types.rend(); iterator ++) {
        auto typeDef = *iterator;
        if(typeDef->kind == TypeKind::Function) {
            return std::static_pointer_cast<FuncType>(typeDef);
        }
    }

    return nullptr;
}

FileModuleType::Ptr CompileContext::curModuleDef() const {
    // The top one must be the FileModuleTypeDef
    return std::static_pointer_cast<FileModuleType>(types[0]);
}
