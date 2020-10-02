#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "node.h"
#include "context.h"
#include "runtime/runtime.h"

class TypeChecker {
public:
    TypeChecker(CompileContext::Ptr context);
    
    void verify(Node::Ptr node);
    
private:
    void verify(SourceBlock::Ptr node);
    void verify(FuncDecl::Ptr node);
    void verify(ConstructorDecl::Ptr node);
    void verify(FuncCallExpr::Ptr node);
    void verify(VarDecl::Ptr node);
    void verify(ParameterClause::Ptr node);
    void verify(Pattern::Ptr node);
    void verify(IdentifierExpr::Ptr node);
    void verify(Type::Ptr node);
    void verify(CodeBlock::Ptr node);
    void verify(ReturnStatement::Ptr node);
    void verify(Expr::Ptr node);
    void verify(LetDecl::Ptr node);
    void verify(LiteralExpr::Ptr node);
    void verify(AssignmentExpr::Ptr node);
    void verify(ParenthesizedExpr::Ptr node);
    void verify(IfStatement::Ptr node);
    void verify(WhileStatement::Ptr node);
    void verify(ArguCallExpr::Ptr node);
    void verify(ClassDecl::Ptr node);
    void verify(SelfExpr::Ptr node);
    void verify(ArrayLiteralExpr::Ptr node);
    void verify(DictLiteralExpr::Ptr node);
    void verify(MemberAccessExpr::Ptr node);
    void verify(MemberFuncCallExpr::Ptr node);
    void verify(SubscriptExpr::Ptr node);
    void verify(ArrayType::Ptr node);
    void verify(PrefixExpr::Ptr node);
    void verify(FileImportDecl::Ptr node);
    
    JrType* typeOf(Node::Ptr node);
    JrType* typeOf(IdentifierExpr::Ptr node);
    JrType* typeOf(Expr::Ptr node);
    JrType* typeOf(LiteralExpr::Ptr node);
    JrType* typeOf(FuncCallExpr::Ptr node);
    JrType* typeOf(ParenthesizedExpr::Ptr node);
    JrType* typeOf(SelfExpr::Ptr node);
    JrType* typeOf(Pattern::Ptr node);
    JrType* typeOf(Type::Ptr node);
    JrType* typeOf(ArrayLiteralExpr::Ptr node);
    JrType* typeOf(DictLiteralExpr::Ptr node);
    JrType* typeOf(MemberAccessExpr::Ptr node);
    JrType* typeOf(MemberFuncCallExpr::Ptr node);
    JrType* typeOf(SubscriptExpr::Ptr node);
    JrType* typeOf(ArrayType::Ptr node);
    JrType* typeOf(PrefixExpr::Ptr node);
    
    // Get Codeblock's return type
    void verifyReturnStatement(SourceBlock::Ptr node);
    void verifyReturnStatement(CodeBlock::Ptr node);
    void verifyReturnStatement(std::vector<Node::Ptr>& statements);
    JrType* returnTypeOf(Node::Ptr node);
    JrType* returnTypeOf(IfStatement::Ptr node);
    JrType* returnTypeOf(CodeBlock::Ptr node);
    JrType* returnTypeOf(FuncCallExpr::Ptr node);
    JrType* returnTypeOf(WhileStatement::Ptr node);
    
    CompileContext::Ptr context;
};

#endif
