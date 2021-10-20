#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "joyeer/compiler/node+visitor.h"
#include "joyeer/compiler/node.h"
#include "joyeer/compiler/context.h"

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
    Node::Ptr visit(AssignExpr::Ptr decl) override;
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

    JrTypeDef::Ptr typeOf(const Node::Ptr& node);
    JrTypeDef::Ptr typeOf(const IdentifierExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const Expr::Ptr& node);
    JrTypeDef::Ptr typeOf(const LiteralExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const FuncCallExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const ParenthesizedExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const SelfExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const Pattern::Ptr& node);
    JrTypeDef::Ptr typeOf(const Type::Ptr& node);
    JrTypeDef::Ptr typeOf(const ArrayLiteralExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const DictLiteralExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const MemberAccessExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const MemberFuncCallExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const SubscriptExpr::Ptr& node);
    JrTypeDef::Ptr typeOf(const ArrayType::Ptr& node);
    JrTypeDef::Ptr typeOf(const PrefixExpr::Ptr& node);
    
    // Get Code-Block's return type
    void verifyReturnStatement(const StmtsBlock::Ptr& node);
    void verifyReturnStatement(std::vector<Node::Ptr>& statements);
    JrTypeDef::Ptr returnTypeOf(const Node::Ptr& node);
    JrTypeDef::Ptr returnTypeOf(const IfStmt::Ptr& node);
    JrTypeDef::Ptr returnTypeOf(const StmtsBlock::Ptr& node);
    JrTypeDef::Ptr returnTypeOf(const FuncCallExpr::Ptr& node);
    JrTypeDef::Ptr returnTypeOf(const WhileStmt::Ptr& node);
    
    CompileContext::Ptr context;
};

#endif
