#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "joyeer/compiler/node+visitor.h"
#include "joyeer/compiler/node+types.h"
#include "context.h"
#include "joyeer/runtime/runtime.h"

class TypeChecker : public NodeVisitor {
public:
    explicit TypeChecker(CompileContext::Ptr context);
    
    virtual Node::Ptr visit(Node::Ptr node);
    
protected:
    Node::Ptr visit(FileModuleDecl::Ptr decl) override;
    Node::Ptr visit(ClassDecl::Ptr classDecl) override;
    Node::Ptr visit(VarDecl::Ptr varDecl) override;
    Node::Ptr visit(Type::Ptr decl) override;
    Node::Ptr visit(FuncCallExpr::Ptr decl) override;
    Node::Ptr visit(MemberFuncCallExpr::Ptr decl) override;
    Node::Ptr visit(ArguCallExpr::Ptr decl) override;
    Node::Ptr visit(LiteralExpr::Ptr decl) override;
    Node::Ptr visit(PrefixExpr::Ptr decl) override;
    Node::Ptr visit(IdentifierExpr::Ptr decl) override;
    Node::Ptr visit(Expr::Ptr decl) override;
    Node::Ptr visit(AssignmentExpr::Ptr decl) override;
    Node::Ptr visit(BinaryExpr::Ptr decl) override;
    Node::Ptr visit(OperatorExpr::Ptr decl) override;
    Node::Ptr visit(ParenthesizedExpr::Ptr decl) override;
    Node::Ptr visit(IfStmt::Ptr decl) override;
    Node::Ptr visit(WhileStmt::Ptr decl) override;
    Node::Ptr visit(StmtsBlock::Ptr decl) override;
    Node::Ptr visit(FuncDecl::Ptr decl) override;
    Node::Ptr visit(ParameterClause::Ptr decl) override;
    Node::Ptr visit(Pattern::Ptr decl) override;
    Node::Ptr visit(ReturnStmt::Ptr decl) override;
    Node::Ptr visit(SelfExpr::Ptr decl) override;
    Node::Ptr visit(ArrayLiteralExpr::Ptr decl) override;
    Node::Ptr visit(DictLiteralExpr::Ptr decl) override;
    Node::Ptr visit(MemberAccessExpr::Ptr decl) override;
    Node::Ptr visit(MemberAssignExpr::Ptr decl) override;
    Node::Ptr visit(SubscriptExpr::Ptr decl) override;
    Node::Ptr visit(ArrayType::Ptr decl) override;
    Node::Ptr visit(FileImportStmt::Ptr decl) override;
    
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
