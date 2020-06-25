#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "node.h"

class IRGen {
public:
    void emit(SourceBlock::Pointer block);
    
protected:
    void emit(Node::Pointer node);
    
    void emit(FuncCallExpr::Pointer node);
    
};

#endif
