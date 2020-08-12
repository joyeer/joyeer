#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "node.h"
#include "context.h"
#include "runtime/runtime.h"

class TypeChecker {
public:
    TypeChecker(CompileContext::Pointer context);
    
    void verify(Node::Pointer node);
    
private:
    void verify(SourceBlock::Pointer node);
    void verify(FuncDecl::Pointer node);
    void verify(ConstructorDecl::Pointer node);
    void verify(FuncCallExpr::Pointer node);
    void verify(VarDecl::Pointer node);
    void verify(ParameterClause::Pointer node);
    void verify(Pattern::Pointer node);
    void verify(IdentifierExpr::Pointer node);
    void verify(Type::Pointer node);
    void verify(CodeBlock::Pointer node);
    void verify(ReturnStatement::Pointer node);
    void verify(Expr::Pointer node);
    void verify(LetDecl::Pointer node);
    void verify(LiteralExpr::Pointer node);
    void verify(AssignmentExpr::Pointer node);
    void verify(ParenthesizedExpr::Pointer node);
    void verify(IfStatement::Pointer node);
    void verify(ArguCallExpr::Pointer node);
    void verify(ClassDecl::Pointer node);
    void verify(SelfExpr::Pointer node);
    void verify(ArrayLiteralExpr::Pointer node);
    void verify(MemberAccessExpr::Pointer node);
    void verify(SubscriptExpr::Pointer node);
    void verify(ArrayType::Pointer node);
    
    JrType* typeOf(Node::Pointer node);
    JrType* typeOf(IdentifierExpr::Pointer node);
    JrType* typeOf(Expr::Pointer node);
    JrType* typeOf(LiteralExpr::Pointer node);
    JrType* typeOf(FuncCallExpr::Pointer node);
    JrType* typeOf(ParenthesizedExpr::Pointer node);
    JrType* typeOf(SelfExpr::Pointer node);
    JrType* typeOf(Pattern::Pointer node);
    JrType* typeOf(Type::Pointer node);
    JrType* typeOf(ArrayLiteralExpr::Pointer node);
    JrType* typeOf(MemberAccessExpr::Pointer node);
    JrType* typeOf(SubscriptExpr::Pointer node);
    JrType* typeOf(ArrayType::Pointer node);
    
    // Get Codeblock's return type
    void verifyReturnStatement(SourceBlock::Pointer node);
    void verifyReturnStatement(CodeBlock::Pointer node);
    void verifyReturnStatement(std::vector<Node::Pointer>& statements);
    JrType* returnTypeOf(Node::Pointer node);
    JrType* returnTypeOf(IfStatement::Pointer node);
    JrType* returnTypeOf(CodeBlock::Pointer node);
    JrType* returnTypeOf(FuncCallExpr::Pointer node);
    
    CompileContext::Pointer context;
};

#endif
