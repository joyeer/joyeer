#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "joyeer/compiler/node+visitor.h"
#include "joyeer/compiler/node+types.h"
#include "context.h"
#include "joyeer/runtime/runtime.h"

class TypeChecker : public NodeVisitor {
public:
    TypeChecker(CompileContext::Ptr context);
    
    virtual Node::Ptr visit(Node::Ptr node);
    
protected:
    virtual Node::Ptr visit(FileModuleDecl::Ptr decl);
    virtual Node::Ptr visit(ClassDecl::Ptr classDecl);
    virtual Node::Ptr visit(VarDecl::Ptr varDecl);
    virtual Node::Ptr visit(Type::Ptr decl);
    virtual Node::Ptr visit(FuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(MemberFuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(ArguCallExpr::Ptr decl);
    virtual Node::Ptr visit(LiteralExpr::Ptr decl);
    virtual Node::Ptr visit(PrefixExpr::Ptr decl);
    virtual Node::Ptr visit(IdentifierExpr::Ptr decl);
    virtual Node::Ptr visit(Expr::Ptr decl);
    virtual Node::Ptr visit(AssignmentExpr::Ptr decl);
    virtual Node::Ptr visit(BinaryExpr::Ptr decl);
    virtual Node::Ptr visit(OperatorExpr::Ptr decl);
    virtual Node::Ptr visit(ParenthesizedExpr::Ptr decl);
    virtual Node::Ptr visit(IfStmt::Ptr decl);
    virtual Node::Ptr visit(WhileStmt::Ptr decl);
    virtual Node::Ptr visit(StmtsBlock::Ptr decl);
    virtual Node::Ptr visit(FuncDecl::Ptr decl);
    virtual Node::Ptr visit(ParameterClause::Ptr decl);
    virtual Node::Ptr visit(Pattern::Ptr decl);
    virtual Node::Ptr visit(ReturnStmt::Ptr decl);
    virtual Node::Ptr visit(SelfExpr::Ptr decl);
    virtual Node::Ptr visit(ArrayLiteralExpr::Ptr decl);
    virtual Node::Ptr visit(DictLiteralExpr::Ptr decl);
    virtual Node::Ptr visit(MemberAccessExpr::Ptr decl);
    virtual Node::Ptr visit(SubscriptExpr::Ptr decl);
    virtual Node::Ptr visit(ArrayType::Ptr decl);
    virtual Node::Ptr visit(FileImportStmt::Ptr decl);
    
    ClassDecl::Ptr processClassDecl(ClassDecl::Ptr decl);

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
    void verifyReturnStatement(StmtsBlock::Ptr node);
    void verifyReturnStatement(std::vector<Node::Ptr>& statements);
    JrType* returnTypeOf(Node::Ptr node);
    JrType* returnTypeOf(IfStmt::Ptr node);
    JrType* returnTypeOf(StmtsBlock::Ptr node);
    JrType* returnTypeOf(FuncCallExpr::Ptr node);
    JrType* returnTypeOf(WhileStmt::Ptr node);
    
    CompileContext::Ptr context;
};

#endif
