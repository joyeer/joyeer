//
// Created by Qing Xu on 2021/10/27.
//

#include "joyeer/compiler/memory+alignment.h"


////////////////////////////////////////////////////////
// VariableLocalRebase
////////////////////////////////////////////////////////

void VariableLocalRebase::align(const ModuleDecl::Ptr& decl) {
    align(std::static_pointer_cast<StmtsBlock>(decl));
}

void VariableLocalRebase::align(const FuncDecl::Ptr &decl) {
    auto funcType = std::static_pointer_cast<FuncType>(decl->getType());
    for(const auto paramType: funcType->paramTypes) {
        paramType->position = base;
        base ++;
    }
    auto stmts = std::static_pointer_cast<StmtsBlock>(decl->codeBlock);
    align(stmts);
}

void VariableLocalRebase::align(const StmtsBlock::Ptr &decl) {
    auto blockType = std::static_pointer_cast<BlockType>(decl->getType());
    blockType->base = base;
    auto startPosition = blockType->base;

    for(auto& localVar : blockType->localVars) {
        localVar->position = startPosition;
        startPosition ++;
    }
    base += static_cast<int>(blockType->localVars.size());

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