#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "joyeer/compiler/node.h"
#include "joyeer/runtime/runtime.h"
#include "joyeer/runtime/instruction.h"
#include "joyeer/runtime/function.h"

class JrCodeWriter;

class IRGen {
public:
    IRGen(CompileContext::Ptr context);
    
    JrModuleClass* emit(SourceBlock::Ptr block);
    
protected:
    void emit(Node::Ptr node);
    void emit(FuncCallExpr::Ptr node);
    void emit(MemberFuncCallExpr::Ptr node);
    void emit(ArguCallExpr::Ptr node);
    void emit(LiteralExpr::Ptr node);
    void emit(LetDecl::Ptr node);
    void emit(VarDecl::Ptr node);
    void emit(PrefixExpr::Ptr node);
    void emit(IdentifierExpr::Ptr node);
    void emit(AssignmentExpr::Ptr node);
    void emit(Expr::Ptr node);
    void emit(OperatorExpr::Ptr node);
    void emit(ParenthesizedExpr::Ptr node);
    void emit(IfStatement::Ptr node);
    void emit(WhileStatement::Ptr node);
    void emit(CodeBlock::Ptr node);
    void emit(FuncDecl::Ptr node);
    void emit(ReturnStatement::Ptr node);
    void emit(MemberAccessExpr::Ptr node);
    
    void emit(ClassDecl::Ptr node);
    void emit(ConstructorDecl::Ptr node);
    void emit(ArrayLiteralExpr::Ptr node);
    void emit(DictLiteralExpr::Ptr node);
    void emit(SubscriptExpr::Ptr node);
    void emit(FileImportDecl::Ptr node);
    
private:
    JrCodeWriter writer;
    CompileContext::Ptr context;
};

#endif