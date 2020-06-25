#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "node.h"
#include "runtime/instruction.h"

class JrCodeWriter;

class IRGen {
public:
    void emit(Node::Pointer node);
    
    
protected:
    void emit(SourceBlock::Pointer block);
    
    void emit(FuncCallExpr::Pointer node);
    
    void emit(ArguCallExpr::Pointer node);
    
    void emit(LiteralExpr::Pointer node);
    
private:
    JrCodeWriter writer;
};

#endif
