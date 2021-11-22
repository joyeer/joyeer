#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "joyeer/compiler/context.h"
#include "joyeer/compiler/node+visitor.h"

class TypeChecker : public NodeVisitor {
public:
    explicit TypeChecker(CompileContext::Ptr context);

    Node::Ptr visit(const Node::Ptr& node) override;
    
protected:
    Node::Ptr visit(const FileModuleDecl::Ptr& decl) override;
    Node::Ptr visit(const ClassDecl::Ptr& classDecl) override;
    Node::Ptr visit(const VarDecl::Ptr& varDecl) override;
    Node::Ptr visit(const TypeIdentifier::Ptr& decl) override;
    Node::Ptr visit(const FuncCallExpr::Ptr& decl) override;
    Node::Ptr visit(const MemberFuncCallExpr::Ptr& decl) override;
    Node::Ptr visit(const ArguCallExpr::Ptr& decl) override;
    Node::Ptr visit(const LiteralExpr::Ptr& decl) override;
    Node::Ptr visit(const PrefixExpr::Ptr& decl) override;
    Node::Ptr visit(const IdentifierExpr::Ptr& decl) override;
    Node::Ptr visit(const Expr::Ptr& decl) override;
    Node::Ptr visit(const AssignExpr::Ptr& decl) override;
    Node::Ptr visit(const BinaryExpr::Ptr& decl) override;
    Node::Ptr visit(const OperatorExpr::Ptr& decl) override;
    Node::Ptr visit(const ParenthesizedExpr::Ptr& decl) override;
    Node::Ptr visit(const IfStmt::Ptr& decl) override;
    Node::Ptr visit(const WhileStmt::Ptr& decl) override;
    Node::Ptr visit(const StmtsBlock::Ptr& decl) override;
    Node::Ptr visit(const FuncDecl::Ptr& decl) override;
    Node::Ptr visit(const ParameterClause::Ptr& decl) override;
    Node::Ptr visit(const Pattern::Ptr& decl) override;
    Node::Ptr visit(const ReturnStmt::Ptr& decl) override;
    Node::Ptr visit(const SelfExpr::Ptr& decl) override;
    Node::Ptr visit(const ArrayLiteralExpr::Ptr& decl) override;
    Node::Ptr visit(const DictLiteralExpr::Ptr& decl) override;
    Node::Ptr visit(const MemberAccessExpr::Ptr& decl) override;
    Node::Ptr visit(const MemberAssignExpr::Ptr& decl) override;
    Node::Ptr visit(const SubscriptExpr::Ptr& decl) override;
    Node::Ptr visit(const ArrayType::Ptr& decl) override;
    Node::Ptr visit(const FileImportStmt::Ptr& decl) override;
    
    ClassDecl::Ptr processClassDecl(ClassDecl::Ptr decl);

    Type::Ptr typeOf(const Node::Ptr& node);
    Type::Ptr typeOf(const IdentifierExpr::Ptr& node);
    Type::Ptr typeOf(const Expr::Ptr& node);
    Type::Ptr typeOf(const LiteralExpr::Ptr& node);
    Type::Ptr typeOf(const FuncCallExpr::Ptr& node);
    Type::Ptr typeOf(const ParenthesizedExpr::Ptr& node);
    Type::Ptr typeOf(const SelfExpr::Ptr& node);
    Type::Ptr typeOf(const Pattern::Ptr& node);
    Type::Ptr typeOf(const TypeIdentifier::Ptr& node);
    Type::Ptr typeOf(const ArrayLiteralExpr::Ptr& node);
    Type::Ptr typeOf(const DictLiteralExpr::Ptr& node);
    Type::Ptr typeOf(const MemberAccessExpr::Ptr& node);
    Type::Ptr typeOf(const MemberFuncCallExpr::Ptr& node);
    Type::Ptr typeOf(const SubscriptExpr::Ptr& node);
    Type::Ptr typeOf(const ArrayType::Ptr& node);
    Type::Ptr typeOf(const PrefixExpr::Ptr& node);
    
    // Get Code-Block's return kind
    void verifyReturnStatement(const StmtsBlock::Ptr& node);
    void verifyReturnStatement(std::vector<Node::Ptr>& statements);
    Type::Ptr returnTypeOf(const Node::Ptr& node);
    Type::Ptr returnTypeOf(const IfStmt::Ptr& node);
    Type::Ptr returnTypeOf(const StmtsBlock::Ptr& node);
    Type::Ptr returnTypeOf(const FuncCallExpr::Ptr& node);
    Type::Ptr returnTypeOf(const WhileStmt::Ptr& node);
    
    CompileContext::Ptr context;
};

#endif
