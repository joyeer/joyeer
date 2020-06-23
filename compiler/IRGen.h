#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "compiler/AST/ast.h"
#include "compiler/syntax/syntax.h"

class IRGen {
public:
    void emit(SourceBlock::Pointer block);
    
protected:
    void emit(Node::Pointer node);
    
    void emit(FunctionCallExpr::Pointer node);
    
};

#endif
