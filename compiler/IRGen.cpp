#include "IRGen.h"

void IRGen::emit(SourceBlock::Pointer block) {
    for(auto& statement: block->statements) {
        
    }
}

void IRGen::emit(Node::Pointer node) {
    switch (node->kind) {
        case functionCallExpr:
            break;
        default:
            break;
    }
}

void IRGen::emit(FunctionCallExpr::Pointer node) {
    
}
