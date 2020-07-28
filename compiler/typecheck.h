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
    void verify(TypeDecl::Pointer node);
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
    
    JrType::Pointer typeOf(Node::Pointer node);
    JrType::Pointer typeOf(IdentifierExpr::Pointer node);
    JrType::Pointer typeOf(Expr::Pointer node);
    JrType::Pointer typeOf(LiteralExpr::Pointer node);
    JrType::Pointer typeOf(FuncCallExpr::Pointer node);
    JrType::Pointer typeOf(ParenthesizedExpr::Pointer node);
    JrType::Pointer typeOf(SelfExpr::Pointer node);
    JrType::Pointer typeOf(Pattern::Pointer node);
    JrType::Pointer typeOf(TypeDecl::Pointer node);
    JrType::Pointer typeOf(ArrayLiteralExpr::Pointer node);
    JrType::Pointer typeOf(MemberAccessExpr::Pointer node);
    
    // Get Codeblock's return type
    void verifyReturnStatement(SourceBlock::Pointer node);
    void verifyReturnStatement(CodeBlock::Pointer node);
    void verifyReturnStatement(std::vector<Node::Pointer>& statements);
    JrType::Pointer returnTypeOf(Node::Pointer node);
    JrType::Pointer returnTypeOf(IfStatement::Pointer node);
    JrType::Pointer returnTypeOf(CodeBlock::Pointer node);
    JrType::Pointer returnTypeOf(FuncCallExpr::Pointer node);
    
    CompileContext::Pointer context;
};

#endif
