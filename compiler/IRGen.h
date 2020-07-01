#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "node.h"
#include "runtime/runtime.h"
#include "runtime/instruction.h"
#include "runtime/function.h"

class JrCodeWriter;

class IRGen {
public:
    
    IRGen();
    
    void emit(Node::Pointer node);
    
    std::vector<Instruction>& getInstructions();
    
    JrFunction::Pointer getFunction();
    
protected:
    void emit(SourceBlock::Pointer block);
    
    void emit(FuncCallExpr::Pointer node);
    
    void emit(ArguCallExpr::Pointer node);
    
    void emit(LiteralExpr::Pointer node);
    
    void emit(ConstDecl::Pointer node);
    
    void emit(VarDecl::Pointer node);
    
    void emit(PrefixExpr::Pointer node);
    
    void emit(IdentifierExpr::Pointer node);
    
    void emit(AssignmentExpr::Pointer node);
    
    void emit(Expr::Pointer node);
    
    void emit(OperatorExpr::Pointer node);
private:
    JrCodeWriter writer;
    
    ScopeVarFinder varFinder;
    
    JrFunction::Pointer function;
    
};

#endif
