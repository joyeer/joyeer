#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "node.h"
#include "context.h"

class TypeChecker {
public:
    TypeChecker(CompileContext::Pointer context);
    
    void verify(Node::Pointer node);
    
private:
    void verify(SourceBlock::Pointer node);
    void verify(FuncDecl::Pointer node);
    void verify(FuncCallExpr::Pointer node);
    void verify(VarDecl::Pointer node);
    
    CompileContext::Pointer context;
};

#endif
