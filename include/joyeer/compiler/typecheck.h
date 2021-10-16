#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "joyeer/compiler/node+visitor.h"
#include "joyeer/compiler/node+types.h"
#include "context.h"
#include "joyeer/runtime/runtime.h"

class TypeChecker : public NodeVisitor {
public:
    explicit TypeChecker(CompileContext::Ptr context);

    Node::Ptr visit(const Node::Ptr& node) override;
    
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

    JrType* typeOf(const Node::Ptr& node);
    JrType* typeOf(const IdentifierExpr::Ptr& node);
    JrType* typeOf(const Expr::Ptr& node);
    JrType* typeOf(const LiteralExpr::Ptr& node);
    JrType* typeOf(const FuncCallExpr::Ptr& node);
    JrType* typeOf(const ParenthesizedExpr::Ptr& node);
    JrType* typeOf(const SelfExpr::Ptr& node);
    JrType* typeOf(const Pattern::Ptr& node);
    JrType* typeOf(const Type::Ptr& node);
    JrType* typeOf(const ArrayLiteralExpr::Ptr& node);
    JrType* typeOf(const DictLiteralExpr::Ptr& node);
    JrType* typeOf(const MemberAccessExpr::Ptr& node);
    JrType* typeOf(const MemberFuncCallExpr::Ptr& node);
    JrType* typeOf(const SubscriptExpr::Ptr& node);
    JrType* typeOf(const ArrayType::Ptr& node);
    JrType* typeOf(const PrefixExpr::Ptr& node);
    
    // Get Code-Block's return type
    void verifyReturnStatement(const StmtsBlock::Ptr& node);
    void verifyReturnStatement(std::vector<Node::Ptr>& statements);
    JrType* returnTypeOf(const Node::Ptr& node);
    JrType* returnTypeOf(const IfStmt::Ptr& node);
    JrType* returnTypeOf(const StmtsBlock::Ptr& node);
    JrType* returnTypeOf(const FuncCallExpr::Ptr& node);
    JrType* returnTypeOf(const WhileStmt::Ptr& node);
    
    CompileContext::Ptr context;
};

#endif
