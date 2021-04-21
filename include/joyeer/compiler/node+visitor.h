#ifndef __joyeer_compiler_node_visitor_h__
#define __joyeer_compiler_node_visitor_h__

#include "joyeer/compiler/node+types.h"

struct NodeVisitor {
protected:
    Node::Ptr visit(Node::Ptr node);
    virtual Node::Ptr visit(FileModuleNode::Ptr decl) = 0;
    virtual Node::Ptr visit(ClassDecl::Ptr classDecl) = 0;
    virtual Node::Ptr visit(LetDecl::Ptr decl) = 0;
    virtual Node::Ptr visit(VarDecl::Ptr varDecl) = 0;
    virtual Node::Ptr visit(ConstructorDecl::Ptr decl) = 0;
    virtual Node::Ptr visit(Type::Ptr decl) = 0;
    virtual Node::Ptr visit(FuncCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(MemberFuncCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ModuleMemberAccessExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ModuleFuncCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ArguCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(LiteralExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(PrefixExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(IdentifierExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(Expr::Ptr decl) = 0;
    virtual Node::Ptr visit(AssignmentExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(BinaryExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(OperatorExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ParenthesizedExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(IfStatement::Ptr decl) = 0;
    virtual Node::Ptr visit(WhileStatement::Ptr decl) = 0;
    virtual Node::Ptr visit(CodeBlock::Ptr decl) = 0;
    virtual Node::Ptr visit(FuncDecl::Ptr decl) = 0;
    virtual Node::Ptr visit(ParameterClause::Ptr decl) = 0;
    virtual Node::Ptr visit(Pattern::Ptr decl) = 0;
    virtual Node::Ptr visit(ReturnStatement::Ptr decl) = 0;
    virtual Node::Ptr visit(SelfExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ArrayLiteralExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(DictLiteralExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(MemberAccessExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(SubscriptExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ArrayType::Ptr decl) = 0;
    virtual Node::Ptr visit(FileImportStatement::Ptr decl) = 0;
};

#endif
