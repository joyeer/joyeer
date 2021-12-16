//
// Created by Qing Xu on 2021/10/27.
//

#include "joyeer/compiler/memory+alignment.h"


////////////////////////////////////////////////////////
// ModuleMemoryAlignment
////////////////////////////////////////////////////////

void ModuleMemoryAlignment::align(const ModuleDecl::Ptr& decl) {
    align(std::static_pointer_cast<StmtsBlock>(decl));
}

void ModuleMemoryAlignment::align(const StmtsBlock::Ptr &decl) {
    auto blockDef = std::static_pointer_cast<BlockType>(decl->getType());
    blockDef->base = base;
    auto startPosition = blockDef->base;

    for(auto& localVar : blockDef->localVars) {
        localVar->position = startPosition;
        startPosition ++;
    }
    base += static_cast<int>(blockDef->localVars.size());

    for(const auto& member : decl->statements) {
        // process the IfStmt
        if(member->kind == SyntaxKind::ifStmt) {
            auto ifStmt = std::static_pointer_cast<IfStmt>(member);
            auto ifBlock = std::static_pointer_cast<StmtsBlock>(ifStmt->ifCodeBlock);
            align(ifBlock);
            if(ifStmt->elseCodeBlock->kind == SyntaxKind::stmtsBlock) {
                auto elseBlock = std::static_pointer_cast<StmtsBlock>(ifStmt->elseCodeBlock);
                align(elseBlock);
            } else if(ifStmt->elseCodeBlock->kind == SyntaxKind::ifStmt) {
                auto ifStmtBlock = std::static_pointer_cast<IfStmt>(ifStmt->elseCodeBlock);
                auto elseBlock = std::static_pointer_cast<StmtsBlock>(ifStmtBlock->ifCodeBlock);
                align(elseBlock);
            } else {
                assert(false);
            }
        }
    }
}