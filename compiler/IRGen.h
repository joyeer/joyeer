#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "node.h"
#include "runtime/runtime.h"
#include "runtime/instruction.h"
#include "runtime/function.h"

class JrCodeWriter;

class IRGen {
public:
    
    IRGen(CompileContext::Pointer context);
    
    void emit(Node::Pointer node);
    
    std::vector<Instruction>& getInstructions();
    
    JrFunction::Pointer getFunction();
    
protected:
    void emit(SourceBlock::Pointer block);
    void emit(FuncCallExpr::Pointer node);
    void emit(ArguCallExpr::Pointer node);
    void emit(LiteralExpr::Pointer node);
    void emit(LetDecl::Pointer node);
    void emit(VarDecl::Pointer node);
    void emit(PrefixExpr::Pointer node);
    void emit(IdentifierExpr::Pointer node);
    void emit(AssignmentExpr::Pointer node);
    void emit(Expr::Pointer node);
    void emit(OperatorExpr::Pointer node);
    void emit(ParenthesizedExpr::Pointer node);
    void emit(IfStatement::Pointer node);
    void emit(CodeBlock::Pointer node);
    void emit(FuncDecl::Pointer node);
    void emit(ReturnStatement::Pointer node);
    void emit(MemberAccessExpr::Pointer node);
    
    void emit(ClassDecl::Pointer node);
    void emit(ConstructorDecl::Pointer node);
    void emit(ArrayLiteralExpr::Pointer node);
    void emit(SubscriptExpr::Pointer node);
    
private:
    JrCodeWriter writer;
    
    // geneated function declaration
    JrFunction::Pointer func;
    
    CompileContext::Pointer context;
};

#endif
