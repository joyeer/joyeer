#include "IRGen.h"

void IRGen::emit(SourceBlock::Pointer block) {
    for(auto& statement: block->statements) {
        
    }
}

void IRGen::emit(Node::Pointer node) {
    switch (node->kind) {
        case functionCallExpr:
            emit(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        default:
            break;
    }
}

void IRGen::emit(FuncCallExpr::Pointer node) {
    
}
